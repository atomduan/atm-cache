#include <atm_core.h>
/*
 * Private
 * */

static void 
atm_init(); 

static void 
atm_service(); 

static atm_bool_t
atm_test_in_test_mod(int argc, char **argv);

static void
atm_svctx_init();


static atm_ctx_t    atm_ctx_type;


/*
 * Public
 * */
atm_ctx_t *atm_ctx = &atm_ctx_type;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_svctx_init()
{
    atm_ctx->dt = atm_dict_new(
            ATM_STR_T, 
            ATM_STR_T, 
            ATM_FREE_DEEP);
    atm_dict_set(atm_ctx->dt, atm_str_new("foo"), NULL);
}


static atm_bool_t
atm_test_in_test_mod(int argc, char **argv)
{
    atm_bool_t res = ATM_FALSE;
    int r = 0;
    if (argc > 1) {
        r = strcmp("test",argv[1]);
        if (r == 0)
            res = ATM_TRUE;
    }
    return res;
}


/*
 * Public
 * */
void 
atm_init() 
{
    atm_log("atm_init enter......");
    atm_svctx_init();
    atm_config_init();
    atm_sig_init();
    atm_hash_init();
    atm_event_init();
    atm_conn_init();
    atm_task_init();
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
    if (atm_test_in_test_mod(argc, argv)) {
#ifdef ATM_UNIT_TEST
        atm_log("Enter in test mode ......");
        atm_test_suit(argc, argv);
#else
        atm_log("ATM_UNIT_TEST macro has not been set "
                "please configure it before building");
#endif
    } else {
        atm_log("Enter in serv mode ......");
        atm_init();
        atm_service();
    }
    return ATM_OK;
}
