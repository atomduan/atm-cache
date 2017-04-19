#include <atm_core.h>


atm_uint_t atm_test_dict_init()
{
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_init", result);
    return result;
}


atm_uint_t atm_test_dict_contains()
{
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_contains", result);
    return result;
}


atm_uint_t atm_test_dict_key_gen()
{
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_key_gen", result);
    return result;
}


atm_uint_t atm_test_dict_get()
{
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_get", result);
    return result;
}


atm_uint_t atm_test_dict_set()
{
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_set", result);
    return result;
}


atm_uint_t atm_test_dict_remove()
{
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_dict_remove", result);
    return result;
}


atm_uint_t atm_test_dict_suit()
{
    atm_dict_init();

    if (!atm_test_dict_contains()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_dict_key_gen()) {
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
