#include <atm_core.h>


/*
 * Private
 * */


static int 
atm_unit_test_proc(int argc, char **argv);


/* ---------------------IMPLEMENTATIONS--------------------------- */


/*
 * Private
 * */


/*
 * Main entry point of unit test system
 */
static atm_int_t 
atm_unit_test_proc(int argc, char **argv) 
{
    atm_log("Hello world unit test ......");
    atm_test_dict_suit();
    return ATM_OK;
}


/*
 * Public
 * */


/*
 * The entry point of unit test system
 */
void
atm_test_suit(int argc, char **argv) 
{ 
    atm_log("atm_test_suit entry....");
    atm_int_t lv = ATM_UNIT_TEST_LV;
    atm_int_t stat = 0;
    pid_t pid = 0;

    if (ATM_UNIT_TEST_SKIP == lv) {
        atm_log("skip unit test....");
        return;
    }
    if (ATM_UNIT_TEST_ONLY == lv 
            || ATM_UNIT_TEST_PREP == lv) {
        atm_log("unit test before all");
        if ((pid = fork()) < 0) {
            atm_log("fork error occure....");
            exit(ATM_FATAL);
        } else if (pid == 0) {
            atm_unit_test_proc(argc, argv);
            exit(ATM_OK);
        }
        wait(&stat);
    }
    if (ATM_UNIT_TEST_ONLY == lv) {
        atm_log("unit test and exit...");
        exit(ATM_OK);
    }
}


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
