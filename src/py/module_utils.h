#include <atm_linux_config.h>
#include <python2.7/Python.h>

#define RES_NANO_SEC    1000000000
#define RES_MICR_SEC    1000000
#define RES_MILI_SEC    1000


typedef struct timespec    atm_timespec_t;


unsigned long
atm_time_mval(atm_timespec_t *rct,
        atm_timespec_t *old);

void
atm_time_mono(atm_timespec_t *ts);
