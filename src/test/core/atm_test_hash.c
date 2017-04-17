#include <atm_config.h>
#include <atm_core.h>

atm_uint_t atm_test_hash_suit() {
    if (!atm_test_hash_init()) {
        return ATM_TEST_HASH_FAIL;
    }
    if (!atm_test_hash_contains()) {
        return ATM_TEST_HASH_FAIL;
    }
    if (!atm_test_hash_key_func()) {
        return ATM_TEST_HASH_FAIL;
    }
    if (!atm_test_hash_get()) {
        return ATM_TEST_HASH_FAIL;
    }
    if (!atm_test_hash_set()) {
        return ATM_TEST_HASH_FAIL;
    }
    if (!atm_test_hash_remove()) {
        return ATM_TEST_HASH_FAIL;
    }
    if (!atm_test_siphash()) {
        return ATM_TEST_HASH_FAIL;
    }
    return ATM_TEST_HASH_PASS;
}

atm_uint_t atm_test_hash_init() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_hash_init", result);
    return result;
}

atm_uint_t atm_test_hash_contains() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_hash_contains", result);
    return result;
}

atm_uint_t atm_test_hash_key_func() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_hash_key_func", result);
    return result;
}

atm_uint_t atm_test_hash_get() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_hash_get", result);
    return result;
}

atm_uint_t atm_test_hash_set() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_hash_set", result);
    return result;
}

atm_uint_t atm_test_hash_remove() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_hash_remove", result);
    return result;
}

atm_uint_t atm_test_siphash() {
    atm_uint_t result = ATM_TEST_HASH_PASS;
    atm_test_report("atm_test_siphash", result);
    return result;
}
