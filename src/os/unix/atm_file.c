#include <atm_core.h>

static int
atm_file_get_type(char *file);

/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static int
atm_file_get_type(char *file)
{
    int res = ATM_FILE_UNKNOWN;
    struct stat statbuf;
    if (lstat(file, &statbuf) == 0) {
        switch(statbuf.st_mode & S_IFMT) {
            case S_IFREG: return ATM_FILE_REG;
            case S_IFDIR: return ATM_FILE_DIR;
            break;
        }
    }
    return res;
}

/*
 * Public
 * */
atm_int_t
atm_file_nonblock(int fd, atm_bool_t nblk)
{
    int flags;
    /*
     * Note that fcntl(2) for F_GETFL
     * and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags=fcntl(fd, F_GETFL))==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_GETFL): %s",
            strerror(errno));
        return ATM_ERROR;
    }

    if (nblk)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd,F_SETFL,flags)==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_SETFL,O_NONBLOCK): %s",
            strerror(errno));
        return ATM_ERROR;
    }
    return ATM_OK;
}


atm_str_t
atm_file_exe_path()
{
    atm_str_t res = NULL;
    char buf[PATH_MAX];
#if (ATM_LINUX)
    int ret = readlink("/proc/self/exe",buf,sizeof(buf));
    if (ret == -1) {
        atm_log_rout(ATM_LOG_FATAL,
            "readlink error [%s]\n", strerror(errno));
    } else {
        buf[ret] = '\0';
    }
#else
    atm_log_rout(ATM_LOG_FATAL,
        "atm_file_exe_path not support"
        "the current OS");
    exit(ATM_ERROR);
#endif
    res = atm_str_new(buf);
    return res;
}


atm_str_t
atm_file_find(char *dir, char *file_name, int type)
{
    atm_str_t res = NULL;
    atm_str_t file_path;

    file_path = atm_file_path_append(dir,file_name);
    if (atm_file_get_type(file_path) == type) {
        res = file_path;
    }
    return res;
}


atm_str_t
atm_file_path_append(char *parent, char *file)
{
    atm_str_t res = atm_str_new(parent);
    res = atm_str_cats(res,"/");
    res = atm_str_cats(res,file);
    return res;
}


void
atm_file_traverse(char *file,atm_file_callback cb,void *argv)
{
    struct dirent *fe = NULL;
    atm_str_t nextfile;
    DIR *d;

    if ((d = opendir(file)) != NULL) {
        while ((fe=readdir(d)) != NULL) {
            if (strcmp(fe->d_name,".") == 0
                    || strcmp(fe->d_name,"..") == 0) {
                continue;
            } else {
                nextfile = atm_file_path_append(file,fe->d_name);
                if(fe->d_type == DT_DIR) {
                    atm_file_traverse(nextfile,cb,argv);
                } else {
                    if (cb != NULL) (*cb)(nextfile,argv);
                }
                atm_str_free(nextfile);
            }
        }
        closedir(d);
    }
}
