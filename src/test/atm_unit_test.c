#include <atm_core.h>


void 
atm_test_report(char *func_name, atm_uint_t res) 
{
    if (ATM_TEST_PASS == res) {
        atm_log("PASS : test func name is [%s]", func_name);
    }
    if (ATM_TEST_FAIL == res) {
        atm_log("FAIL : test func name is [%s]", func_name);
    }
}


/*
 * Main entry point of unit test system
 */
atm_int_t 
atm_unit_test_proc(int argc, char **argv) 
{
    atm_log("Hello world unit test ......");
    atm_test_dict_suit();
    return ATM_OK;
}
