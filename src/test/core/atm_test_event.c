#include <atm_core.h>


atm_uint_t
atm_test_event_suit()
{
    if (!ATM_TEST_EVENT_ON) {
        atm_log("SKIP atm_test_event_suit");
        return ATM_TEST_PASS;
    }
    atm_event_init();

    if (!atm_test_event_add_listen()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_event_add_conn()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_event_del_conn()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_event_add_listen()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_event_add_listen", res);
    return res;
}


atm_uint_t
atm_test_event_add_conn()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_event_add_conn", res);
    return res;
}


atm_uint_t
atm_test_event_del_conn()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_event_del_conn", res);
    return res;
}
