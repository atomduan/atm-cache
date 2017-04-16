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

static void atm_signal_handler(int signo) {
    pid_t pid;
    int status;
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

static void atm_initialize() {
    printf("atm_initialize enter......\n");
    atm_signal_register();
    atm_hash_init_siphash_seed();
}

static void atm_service() {
    printf("atm_service enter......\n");
}

static void atm_unit_test_suit(int argc, char **argv) { 
    int status;
    pid_t pid;
    if ((pid = fork()) < 0) {
        printf("fork error occure....\n");
        exit(1);
    } else if (pid == 0) {
        atm_unit_test_proc(argc, argv);
        exit(0);
    }
    wait(&status);
}

int main(int argc, char **argv) {
    printf("Hello world ......\n");
#ifdef ATM_UNIT_TEST
    atm_unit_test_suit(argc, argv);
#endif
    atm_initialize();
    atm_service();
    return 0;
}

