#ifndef _ATM_TIME_H_INCLUDED_
#define _ATM_TIME_H_INCLUDED_

#include <atm_core.h>

#define ATM_TIME_NANO_RES    1000000000
#define ATM_TIME_MICR_RES    1000000
#define ATM_TIME_MILI_RES    1000

struct atm_time_pair_s {
    atm_time_spec_t  ts[2];
    atm_int_t        curr;
};

atm_time_pair_t *
atm_time_pair_new();

void
atm_time_mark(atm_time_pair_t *tp);

atm_ulong_t
atm_time_mval(atm_time_pair_t *tp);

void
atm_time_mono(atm_time_spec_t *ts);

atm_str_t
atm_time_strftime_now();


#endif /* _ATM_TIME_H_INCLUDED_ */
