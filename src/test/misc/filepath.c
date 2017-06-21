#include <atm_linux_config.h>
#include <misc_utils.h>
#include <libgen.h>

int main(int argc, char **argv)
{
    char *bp = argv[0];
    char *dn = dirname(bp);
    char buf[100];
    int ret;

    printf("dir name is %s\n", dn);
    ret = readlink(bp,buf,sizeof(buf));
    if (ret == -1) {
        printf("readlink error [%s]\n", strerror(errno));
    }
    printf("readlink name is %s\n", buf);
}
