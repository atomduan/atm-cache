/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/28/2017 09:40:41 PM
 *       Compiler:  gcc
 *
 *         Author:  JUNTAO DUAN (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <atm_core.h>

static void atm_signal_handler(atm_int_t signo) 
{
    pid_t pid;
    atm_int_t status;
    if (signal(SIGCLD, atm_signal_handler) == SIG_ERR) {
        atm_log("signal error");
        exit(ATM_OK);
    }
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        atm_log("wait a sub process");
    }
}


static void atm_signal_register() 
{
    if (signal(SIGCLD, atm_signal_handler) == SIG_ERR) {
        atm_log("sgnal error in in init");
        exit(ATM_OK);
    }
}


#ifdef ATM_UNIT_TEST
static void atm_unit_test_suit(atm_int_t argc, atm_str_t *argv) 
{ 
    atm_log("atm_unit_test_suit entry....");
    atm_int_t level = ATM_UNIT_TEST_LV;
    atm_int_t status;
    pid_t pid;

    if (ATM_UNIT_TEST_SKIP == level) {
        atm_log("atm_service skip unit test....");
        return;
    }
    if (ATM_UNIT_TEST_ONLY == level || ATM_UNIT_TEST_PREP == level) {
        atm_log("atm_service unit test before all");
        if ((pid = fork()) < 0) {
            atm_log("fork error occure....");
            exit(ATM_FATAL);
        } else if (pid == 0) {
            atm_unit_test_proc(argc, argv);
            exit(ATM_OK);
        }
        wait(&status);
    }
    if (ATM_UNIT_TEST_ONLY == level) {
        atm_log("atm_service unit test and exit...");
        exit(ATM_OK);
    }
}
#endif


static void atm_initialize() 
{
    atm_log("atm_initialize enter......");
    atm_signal_register();
    atm_dict_init();
}


static void atm_service() 
{
    atm_log("atm_service enter......");
}


int main(int argc,  char **argv) 
{
#ifdef ATM_UNIT_TEST
    atm_unit_test_suit(argc, argv);
#endif
    atm_log("Hello world ......");
    atm_initialize();
    atm_service();
    return ATM_OK;
}
