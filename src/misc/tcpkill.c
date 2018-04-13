#include <atm_linux_config.h>
#include <misc_utils.h>
#include <pcap.h>

static void
err(int error_code, char *error_msg)
{
    printf("[ERROR] error_code->%d, error_msg->%s", error_code, error_msg);
}

int main(int argc, char **argv)
{
    pcap_t *pd = NULL;
    printf("not implemented yet......pd point%p", pd);
    return 0;
}

/* from tcpdump util.c. */
char *
copy_argv(char **argv)
{
    char **p, *buf, *src, *dst;
    u_int len = 0;
    
    p = argv;
    if (*p == 0)
        return (0);
    
    while (*p)
        len += strlen(*p++) + 1;
    
    if ((buf = (char *)malloc(len)) == NULL)
        err(1, "copy_argv: malloc");
    
    p = argv;
    dst = buf;
    
    while ((src = *p++) != NULL) {
        while ((*dst++ = *src++) != '\0')
            ;
        dst[-1] = ' ';
    }
    dst[-1] = '\0';
    return (buf);
}
