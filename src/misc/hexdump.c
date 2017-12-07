#include <atm_linux_config.h>
#include <misc_utils.h>


static char *hexdict="0123456789abcdef";


static char *
atm_util_hexdump(void *ptr, size_t len)
{
    uint8_t *p = (uint8_t *)ptr;
    //char *res = calloc(1,len*2 + 2);
    char *res = malloc(len*2 + 2);
    size_t i;
    char *s = res;
    *s++ = '0';
    *s++ = 'x';
    for (i=0; i<len; i++) {
        int b = p[i];
        *s++ = hexdict[(b & 0x000000f0)>>4];
        *s++ = hexdict[(b & 0x0000000f)];
    }
    return res;
}

int main(int argc, char **argv)
{
    int i = 1;
    char *s;

    s = atm_util_hexdump(&i, 8);
    printf("hexdump is %s\n",s);
    return 0;
}
