#include <atm_core.h>


static void
atm_test_client_init();


/* -------------IMPL-------------- */


static void
atm_test_client_init()
{
    atm_client_init();
}


atm_uint_t
atm_test_client_suit()
{
    if (!ATM_TEST_CLIENT_ON) {
        atm_log("SKIP atm_test_client_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_client_suit");
    }

    atm_test_client_init();
    if (!atm_test_client_smoke()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_client_smoke()
{
    atm_log("atm_test_client_smoke process......");
    char *addr = "127.0.0.1";
    int port = 8088;
    int timeout = 3;

    int buff_size = 1024;
    char buff[buff_size];
    memset(buff, 0, buff_size);

    atm_client_t *c;

    c = atm_client_new(addr, port, timeout);
    atm_client_send(c, "test");
    atm_client_recv(c, buff, buff_size);
    printf("recv buff is %s\n", buff);
    return ATM_TEST_PASS;
}
