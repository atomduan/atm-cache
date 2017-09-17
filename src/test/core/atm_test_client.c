#include <atm_core.h>


/* -------------IMPL-------------- */
atm_uint_t
atm_test_client_suit()
{
    if (!ATM_TEST_CLIENT_ON) {
        atm_log("SKIP atm_test_client_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_client_suit");
    }

    if (!atm_test_client_smoke()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}

atm_uint_t
atm_test_client_smoke()
{
    atm_log("atm_test_client_smoke process......");
    return ATM_TEST_PASS;
}
