#include <atm_linux_config.h>
#include <misc_utils.h>
#include <libgen.h>

int main(int argc, char **argv)
{
    char *bp = argv[0];
    char *dn = dirname(bp);
    char buf[PATH_MAX];
    int ret;

    printf("PATH MAX is %d\n", PATH_MAX);
    printf("argv[0] dir is %s\n", dn);

    ret = readlink("/proc/self/exe",buf,sizeof(buf));
    if (ret == -1) {
        printf("readlink error [%s]\n", strerror(errno));
    } else {
        buf[ret] = '\0';
    }
    printf("%s\n", buf);
}
