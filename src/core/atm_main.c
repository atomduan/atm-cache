#include <atm_core.h>
/*
 * Private
 * */
static atm_bool_t
atm_test_in_test_mod(int argc, char **argv);
static void
atm_service_init();
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
atm_service_init()
{
    atm_dict_init();
    atm_ctx_init();
    atm_sig_init();
    atm_blk_init();
    atm_event_init();
    atm_conn_init();
    atm_task_init();
    atm_cmd_init();
}


static void
atm_service()
{
    printf("atm_service started......! \n"
           "more info can be check in logs......\n");
    for (;;) {
        atm_event_routine();
    }
}


int
main(int argc, char **argv)
{
    atm_config_init(argc,argv);
    atm_log_init();

    if (atm_test_in_test_mod(argc, argv)) {
#ifdef ATM_UNIT_TEST
        printf("Enter in test mode ......! \n"
               "please check info in logs......\n");
        atm_test_suit(argc, argv);
#else
        printf("ATM_UNIT_TEST macro has not been set "
               "please configure it before building......\n");
#endif
    } else {
        atm_service_init();
        atm_service();
    }
    return ATM_OK;
}
