#include <atm_core.h>
/*
 * Private
 * */
static atm_bool_t
atm_test_in_test_mod(int argc, char **argv);
static void
atm_init(int argc, char **argv);
static void
atm_service();


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
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


static void
atm_init(int argc, char **argv)
{
    atm_config_init(argc,argv);
    atm_dict_init();
    atm_ctx_init();
    atm_sig_init();
    atm_event_init();
    atm_conn_init();
    atm_task_init();
    atm_cmd_init();
}


static void
atm_service()
{
    atm_log("atm_service enter......");
    for (;;) {
        atm_event_routine();
    }
}


int
main(int argc, char **argv)
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
        atm_init(argc,argv);
        atm_service();
    }
    return ATM_OK;
}
