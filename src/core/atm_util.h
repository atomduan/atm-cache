#ifndef _ATM_UTIL_H_INCLUDED_
#define _ATM_UTIL_H_INCLUDED_

#define atm_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))
#define ATM_UTIL_POWER_INIT_SIZE 4

atm_uint_t
atm_util_next_power(atm_uint_t num);

void
atm_util_debug_hook(void *arg);
#endif /* _ATM_UTIL_H_INCLUDED_ */
