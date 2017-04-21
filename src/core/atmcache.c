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


static void 
atm_sig_hdl(atm_int_t signo) 
{
    pid_t pid = 0;
    atm_int_t stat = 0;

    if (signal(SIGCLD, atm_sig_hdl) == SIG_ERR) {
        atm_log("signal error");
        exit(ATM_OK);
    }
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        atm_log("wait for sub processes");
    }
}


static void
atm_sig_init() 
{
    if (signal(SIGCLD, atm_sig_hdl) == SIG_ERR) {
        atm_log("sgnal error in init");
        exit(ATM_OK);
    }
}


#ifdef ATM_UNIT_TEST
static void
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
#endif


static void 
atm_init() 
{
    atm_log("atm_init enter......");
    atm_list_init();
    atm_dict_init();
    atm_sig_init();
}


static void 
atm_service() 
{
    atm_log("atm_service enter......");
}


int 
main(int argc,  char **argv) 
{
#ifdef ATM_UNIT_TEST
    atm_test_suit(argc, argv);
#endif
    atm_log("Hello world ......");
    atm_init();
    atm_service();
    return ATM_OK;
}
