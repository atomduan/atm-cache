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


atm_long_t
atm_util_memtoll(const char *p, int *err)
{
    const char *u;
    char buf[128];
    long mul; /* unit multiplier */
    long long val;
    unsigned int digits;

    if (err) *err = 0;

    /* Search the first non digit character. */
    u = p;
    if (*u == '-') u++;
    while(*u && isdigit(*u)) u++;
    if (*u == '\0' || !strcasecmp(u,"b")) {
        mul = 1;
    } else if (!strcasecmp(u,"k")) {
        mul = 1000;
    } else if (!strcasecmp(u,"kb")) {
        mul = 1024;
    } else if (!strcasecmp(u,"m")) {
        mul = 1000*1000;
    } else if (!strcasecmp(u,"mb")) {
        mul = 1024*1024;
    } else if (!strcasecmp(u,"g")) {
        mul = 1000L*1000*1000;
    } else if (!strcasecmp(u,"gb")) {
        mul = 1024L*1024*1024;
    } else {
        if (err) *err = 1;
        return 0;
    }

    /* Copy the digits into a buffer, we'll use strtoll() to convert
     * the digit (without the unit) into a number. */
    digits = u-p;
    if (digits >= sizeof(buf)) {
        if (err) *err = 1;
        return 0;
    }
    memcpy(buf,p,digits);
    buf[digits] = '\0';

    char *endptr;
    errno = 0;
    val = strtoll(buf,&endptr,10);
    if ((val == 0 && errno == EINVAL) || *endptr != '\0') {
        if (err) *err = 1;
        return 0;
    }
    return val*mul;
}


char *
atm_util_hexdump(void *ptr, size_t len)
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
