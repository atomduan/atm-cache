#include <atm_core.h>
/*
 * Private
 * */
static void 
atm_sig_handle(atm_int_t signo);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void 
atm_sig_handle(atm_int_t signo) 
{
    pid_t pid = 0;
    atm_int_t stat = 0;

    if (signal(SIGCLD, atm_sig_handle) 
            == SIG_ERR) {
        atm_log("signal error");
        exit(ATM_OK);
    }
    while ((pid = waitpid(-1, &stat, WNOHANG)) 
            > 0) {
        atm_log("wait for sub processes");
    }
}


/*
 * Public
 * */
void
atm_sig_init() 
{
    if (signal(SIGCLD, atm_sig_handle) 
            == SIG_ERR) {
        atm_log("sgnal error in init");
        exit(ATM_OK);
    }
}
