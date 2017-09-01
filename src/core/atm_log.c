#include <atm_core.h>
/*
 * Private
 * */
static const char *
atm_log_strlv(atm_uint_t level);
static void
atm_log_rout_raw(atm_uint_t lv, char *msg);


static const char * ATM_LOG_LV_ENUM[] = {
    "OFF",
    "ALL",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};

static FILE *logpath = NULL;
static FILE *errpath = NULL;

/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static const char *
atm_log_strlv(atm_uint_t level)
{
    atm_uint_t lvs_size = 0;
    lvs_size = sizeof(ATM_LOG_LV_ENUM)/sizeof(char *);
    if (level >= lvs_size)
        level = lvs_size - 1;
    return ATM_LOG_LV_ENUM[level];
}


static void
atm_log_rout_raw(atm_uint_t lv, char *msg)
{
    atm_uint_t llv;
    atm_str_t now;

    llv = atm_config->loglevel;
    now = atm_time_strftime_now();
    if (msg != NULL) {
        if (llv && lv) {
            if (llv <= lv) {
                if (!logpath) {
                    fprintf(stdout,"[%s] %s : %s\n",
                        atm_log_strlv(lv),now,msg);
                    fflush(stdout);
                } else {
                    fprintf(logpath,"[%s] %s : %s\n",
                        atm_log_strlv(lv),now,msg);
                    fflush(logpath);
                }
            }
        }
    }
    atm_str_free(now);
}


/*
 * Public
 * */
void
atm_log_init()
{
    int n;
    atm_str_t exepath;
    char *exedir;

    atm_str_t logfile;
    char buf[PATH_MAX];

    atm_str_t logpath_str;
    atm_str_t errpath_str;

    logfile = atm_config->logfile;
    exepath = atm_file_exe_path();
    exedir = dirname(exepath);

    if (!logpath && !atm_str_isempty(logfile)) {
        n = sprintf(buf,"../logs/%s",logfile);
        buf[n] = '\0';
        logpath_str = atm_file_path_append(exedir,buf);
        logpath = fopen(logpath_str, "a+");
        atm_str_free(logpath_str);
    }

#ifdef ATM_ERROR_LOG_PATH
    if (!errpath && !atm_str_isempty(ATM_ERROR_LOG_PATH)) {
        memset(buf,0,PATH_MAX);
        n = sprintf(buf,"../%s",ATM_ERROR_LOG_PATH);
        buf[n] = '\0';
        errpath_str = atm_file_path_append(exedir,buf);
        errpath = fopen(errpath_str, "a+");
        atm_str_free(errpath_str);
    }
#endif

    atm_str_free(exepath);
}


int
atm_log_parse_level(char *lv)
{
    int res = -1;
    int size;
    int i;

    atm_str_t slv = atm_str_new(lv);
    if (slv != NULL) {
        atm_str_toupper(slv);
        size = sizeof(ATM_LOG_LV_ENUM)/sizeof(char *);
        for (i=0; i<size; i++) {
            if (!strcmp(slv,ATM_LOG_LV_ENUM[i])) {
                res = i;
                break;
            }
        }
    }
    return res;
}


void
atm_log(char *fmt, ...)
{
    atm_uint_t size = ATM_LOG_MAXLEN;
    char msg[size];
    memset(msg,ATM_MEM_ZERO,size);

    va_list args;
    va_start(args,fmt);
    vsnprintf(msg,size,fmt,args);
    va_end(args);

    atm_log_rout_raw(ATM_LOG_INFO,msg);
}


void
atm_log_err(char *fmt, ...)
{
    atm_uint_t size = ATM_LOG_MAXLEN;
    atm_str_t now;
    char msg[size];
    memset(msg,ATM_MEM_ZERO,size);

    va_list args;
    va_start(args,fmt);
    vsnprintf(msg,size,fmt,args);
    va_end(args);

    now = atm_time_strftime_now();
    if (msg != NULL) {
        if (!errpath) {
            fprintf(stderr,"%s : %s\n",now,msg);
            atm_log_fflush();
        } else {
            fprintf(errpath,"%s : %s\n",now,msg);
        }
    }
    atm_str_free(now);
}


void
atm_log_rout(atm_uint_t level, char *fmt, ...)
{
    atm_uint_t size = ATM_LOG_MAXLEN;
    char msg[size];
    memset(msg,ATM_MEM_ZERO,size);

    va_list args;
    va_start(args,fmt);
    vsnprintf(msg,size,fmt,args);
    va_end(args);

    atm_log_rout_raw(level, msg);
}


void
atm_log_fflush()
{
    /* TODO SHOULD FLUSH ALL REGULAR FILE */
    fflush(stderr);
    fflush(stdout);
}
