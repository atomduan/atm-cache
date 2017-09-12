#include <atm_linux_config.h>
#include <module_utils.h>


/*
 * A micro seconds diff will return
 */
unsigned long
atm_time_mval(atm_timespec_t *rct,
        atm_timespec_t *old)
{
    unsigned long res = 0;
    unsigned long sec_diff = 0;

    unsigned long nr = 0;
    unsigned long no = 0;
    if (rct->tv_sec >= old->tv_sec) {
        sec_diff = rct->tv_sec - old->tv_sec;
        nr = sec_diff * RES_NANO_SEC + rct->tv_nsec;
        no = old->tv_nsec;
        res = ((nr-no)*RES_MILI_SEC)/RES_NANO_SEC;
    }
    return res;
}


void
atm_time_mono(atm_timespec_t *ts)
{
    clock_gettime(CLOCK_MONOTONIC_COARSE, ts);
}
