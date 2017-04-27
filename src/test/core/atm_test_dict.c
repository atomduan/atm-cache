#include <atm_core.h>


atm_uint_t
atm_test_dict_suit()
{
    if (!ATM_TEST_DICT_ON) {
        atm_log("SKIP atm_test_dict_suit");
        return ATM_TEST_PASS;
    }
    atm_dict_init();

    if (!atm_test_dict_contains()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_dict_get()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_dict_set()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_dict_remove()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_dict_init()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_init", res);
    return res;
}


atm_uint_t
atm_test_dict_contains()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_contains", res);
    return res;
}


atm_uint_t
atm_test_dict_get()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_get", res);
    return res;
}


atm_uint_t
atm_test_dict_set()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_set", res);
    return res;
}


atm_uint_t
atm_test_dict_remove()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_remove", res);
    return res;
}
