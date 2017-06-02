#ifndef _ATM_UTIL_H_INCLUDED_
#define _ATM_UTIL_H_INCLUDED_

#define atm_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))
#define ATM_UTIL_POWER_INIT_SIZE 4

atm_uint_t
atm_util_next_power(atm_uint_t num);

atm_long_t
atm_util_memtoll(const char *p, int *err);

char *
atm_util_hexdump(void *ptr, size_t len);

#endif /* _ATM_UTIL_H_INCLUDED_ */
