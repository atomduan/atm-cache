#include <atm_core.h>


static void
atm_test_dict_init();

static atm_dict_t *dt;


/* -------------IMPL-------------- */


static void
atm_test_dict_init()
{
    atm_dict_init();
    dt = atm_dict_new(
            ATM_STR_T, 
            ATM_STR_T, 
            ATM_FREE_DEEP);
}


atm_uint_t
atm_test_dict_suit()
{
    if (!ATM_TEST_DICT_ON) {
        atm_log("SKIP atm_test_dict_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_dict_suit");
    }

    atm_test_dict_init();

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
atm_test_dict_contains()
{
    atm_log("atm_test_dict_contains......");
    atm_uint_t res = ATM_TEST_PASS;

    atm_log("1");
    atm_str_t *k = atm_str_new("foo");
    atm_str_t *v = atm_str_new("foo--value");
    atm_str_t *k2 = atm_str_new("bar");
    atm_log("2");

    atm_dict_set(dt, k, v);
    atm_log("3");
    
    if (atm_dict_contains(dt, k2)) {
        atm_log("a");
        res = ATM_TEST_FAIL; goto fin;
    }

    atm_log("4");
    if (!atm_dict_contains(dt, k)) {
        atm_log("b");
        res = ATM_TEST_FAIL; goto fin;
    }
    atm_log("5");
    atm_log("dssssssss");
fin:
    atm_log("6");
    atm_log("c");
    atm_test_report("atm_test_dict_contains",res);
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
