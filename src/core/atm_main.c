#include <atm_core.h>


/*
 * Private
 * */


static atm_ctx_t atm_ctx_struct = {

};


/*
 * Public
 * */


atm_ctx_t *atm_ctx = &atm_ctx_struct;


/* ---------------------IMPLEMENTATIONS--------------------------- */


/*
 * Public
 * */


void 
atm_init() 
{
    atm_log("atm_init enter......");
    atm_sig_init();
    atm_hash_init();
    atm_event_init();
    atm_conn_init();
}


void 
atm_service() 
{
    atm_log("atm_service enter......");
    for (;;) {
        atm_event_routine();
    }
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
