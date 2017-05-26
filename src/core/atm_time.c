#include <atm_core.h>
/*
 * Private
 * */
void
atm_time_pair_reset(atm_time_pair_t *tp);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
void
atm_time_pair_reset(atm_time_pair_t *tp)
{
    if (tp != NULL) {
        memset(tp, ATM_MEM_ZERO, sizeof(*tp));
    }
}


/*
 * Public
 * */
atm_time_pair_t *
atm_time_pair_new()
{
    atm_time_pair_t *res = NULL;
    res = atm_alloc(sizeof(atm_time_pair_t));
    res->curr = 0;
    return res;
}


void
atm_time_mark(atm_time_pair_t *tp)
{
    atm_int_t i;
    i = tp->curr % 2;
    atm_time_mono(&tp->ts[i]);
    tp->curr++;
}


atm_ulong_t
atm_time_mval(atm_time_pair_t *tp)
{
    atm_ulong_t res = 0;
    atm_time_spec_t *new_one;
    atm_time_spec_t *old_one;
    atm_ulong_t sec_diff;
    atm_ulong_t nr;
    atm_ulong_t no;

    old_one = &tp->ts[0];
    new_one = &tp->ts[1];
    if (new_one->tv_sec >= old_one->tv_sec) {
        sec_diff = new_one->tv_sec - old_one->tv_sec;
        nr = sec_diff * ATM_TIME_NANO_RES + new_one->tv_nsec;
        no = old_one->tv_nsec;
        res = ((nr-no)*ATM_TIME_MILI_RES)/ATM_TIME_NANO_RES;
    }
    atm_time_pair_reset(tp);
    return res;
}


void
atm_time_mono(atm_time_spec_t *ts)
{
    clock_gettime(
        CLOCK_MONOTONIC_COARSE, ts);
}
