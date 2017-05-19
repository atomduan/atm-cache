#include <atm_core.h>


/* -------------IMPL-------------- */

atm_uint_t
atm_test_time_suit()
{
    if (!ATM_TEST_TIME_ON) {
        atm_log("SKIP atm_test_time_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_time_suit");
    }

    if (!atm_test_time_pair()) {
        return ATM_TEST_FAIL;
    }

    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_time_pair()
{
    atm_log("atm_test_str_split");
    atm_time_pair_t *tp = NULL;
    atm_ulong_t diff = 0;
    tp = atm_time_pair_new();

    atm_time_mark(tp);
    usleep(12345);
    atm_time_mark(tp);
    diff = atm_time_mval(tp);
    atm_log("time pair, 1st diff is [%lu]", diff);

    atm_time_mark(tp);
    usleep(43214);
    atm_time_mark(tp);
    diff = atm_time_mval(tp);
    atm_log("time pair, 2nd diff is [%lu]", diff);

    return ATM_TEST_PASS;
}
