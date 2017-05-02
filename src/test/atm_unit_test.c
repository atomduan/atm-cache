#include <atm_core.h>
/*
 * Private
 * */
static int 
atm_test_suit_proc(int argc, char **argv);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_int_t 
atm_test_suit_proc(int argc, char **argv) 
{
    atm_log("enter ");
    atm_test_siphash_suit();
    atm_test_dict_suit();
    atm_test_event_suit();
    return ATM_OK;
}


/*
 * Public
 * */
void
atm_test_suit(int argc, char **argv) 
{ 
    atm_int_t stat = 0;
    pid_t pid = 0;

    atm_log("atm_test_suit entry....");
    if ((pid = fork()) < 0) {
        atm_log("fork error occure....");
        exit(ATM_FATAL);
    } else if (pid == 0) {
        atm_test_suit_proc(argc, argv);
        exit(ATM_OK);
    }
    wait(&stat);
    atm_log("test complete, exit....");
}


void 
atm_test_report(char *func_name, atm_uint_t res) 
{
    if (ATM_TEST_PASS == res) {
        atm_log("PASS : test func name is [%s]",
                func_name);
    }
    if (ATM_TEST_FAIL == res) {
        atm_log("FAIL : test func name is [%s]",
                func_name);
    }
}
