#include <atm_config.h>
#include <atm_core.h>

void atm_test_report(atm_str_t func_name, atm_uint_t result) {
    if (ATM_TEST_PASS == result) {
        atm_log("PASS : test func name is [%s]", func_name);
    }
    if (ATM_TEST_FAIL == result) {
        atm_log("FAIL : test func name is [%s]", func_name);
    }
}

/*
 * Main entry point of unit test system
 */
int atm_unit_test_proc(int argc, char **argv) {
    atm_log("Hello world unit test ......");
    atm_test_hash_suit();
    return 0;
}
