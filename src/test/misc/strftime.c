#include <atm_linux_config.h>
#include <misc_utils.h>

int main(int argc, char **argv)
{
    char buf[64];
    char timestr[128];

    struct timeval tv;
    gettimeofday(&tv,NULL);
    strftime(buf,sizeof(buf),"%F %H:%M:%S",localtime(&tv.tv_sec));
    snprintf(timestr,sizeof(timestr),"%s.%-3ld",buf,tv.tv_usec/1000);
    printf("time is : %s", timestr);
    return 0;
}
