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
#include <atm_config.h>
#include <atm_core.h>

static void atm_signal_handler(atm_int_t signo) {
    pid_t pid;
    atm_int_t status;
    if (signal(SIGCLD, atm_signal_handler) == SIG_ERR) {
        printf("signal error\n");
        exit(0);
    }
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("wait a sub process\n");
    }
}

static void atm_signal_register() {
    if (signal(SIGCLD, atm_signal_handler) == SIG_ERR) {
        printf("sgnal error in in init\n");
        exit(0);
    }
}

#ifdef ATM_UNIT_TEST
static void atm_unit_test_suit(atm_int_t argc, atm_str_t *argv) { 
    printf("atm_unit_test_suit entry....\n");
    atm_int_t mod = ATM_UNIT_TEST_LV;
    atm_int_t status;
    pid_t pid;

    if (ATM_UNIT_TEST_SKIP == mod) {
        printf("atm_service skip unit test....\n");
        return;
    }
    if (ATM_UNIT_TEST_ONLY == mod || ATM_UNIT_TEST_PREP == mod) {
        printf("atm_service unit test before all\n");
        if ((pid = fork()) < 0) {
            printf("fork error occure....\n");
            exit(1);
        } else if (pid == 0) {
            atm_unit_test_proc(argc, argv);
            exit(0);
        }
        wait(&status);
    }
    if (ATM_UNIT_TEST_ONLY == mod) {
        printf("atm_service unit test and exit...\n");
        exit(0);
    }
}
#endif

static void atm_initialize() {
    printf("atm_initialize enter......\n");
    atm_signal_register();
    atm_hash_init();
}

static void atm_service() {
    printf("atm_service enter......\n");
}

atm_int_t main(atm_int_t argc,  atm_str_t *argv) {
#ifdef ATM_UNIT_TEST
    atm_unit_test_suit(argc, argv);
#endif
    printf("Hello world ......\n");
    atm_initialize();
    atm_service();
    return 0;
}

