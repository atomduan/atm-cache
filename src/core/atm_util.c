#include <atm_core.h>


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
