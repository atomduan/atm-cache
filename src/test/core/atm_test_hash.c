#include <atm_config.h>
#include <atm_core.h>

atm_uint_t atm_test_hash_init() {
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_hash_init", result);
    return result;
}


atm_uint_t atm_test_hash_contains() {
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_hash_contains", result);
    return result;
}


atm_uint_t atm_test_hash_key_gen() {
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_hash_key_gen", result);
    return result;
}


atm_uint_t atm_test_hash_get() {
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_hash_get", result);
    return result;
}


atm_uint_t atm_test_hash_set() {
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_hash_set", result);
    return result;
}


atm_uint_t atm_test_hash_remove() {
    atm_uint_t result = ATM_TEST_PASS;
    atm_test_report("atm_test_hash_remove", result);
    return result;
}


atm_uint_t atm_test_hash_suit() {
    atm_hash_init();

    if (!atm_test_hash_contains()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_hash_key_gen()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_hash_get()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_hash_set()) {
        return ATM_TEST_FAIL;
    }
    if (!atm_test_hash_remove()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}
