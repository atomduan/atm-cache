#include <atm_core.h>


static char *hexdict="0123456789abcdef";


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
atm_uint_t
atm_util_next_power(atm_uint_t num)
{
    atm_uint_t res = ATM_UTIL_POWER_INIT_SIZE;
    while (ATM_TRUE) {
        if (res >= num) return res;
        res *= 2; 
    }
    return res;
}


char *
atm_debug_hexdump(void *ptr, size_t len)
{
    char *res = calloc(1,len*2 + 2);
    uint8_t *p = (uint8_t *)ptr;
    int b; 
    size_t i;

    char *s = res;
    *s++ = '0';
    *s++ = 'x';

    for (i=0; i<len; i++) {
        b = p[i]; 
        *s++ = hexdict[(b & 0x000000f0)>>4];
        *s++ = hexdict[(b & 0x0000000f)];
    }
    return res;
}
