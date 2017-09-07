#include <atm_core.h>
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
atm_client_t *
atm_client_new(char *addr, int port, int timeout)
{
    atm_client_t *res = NULL;
    int sock;
    struct timeval tov;

    tov.tv_sec = timeout;
    tov.tv_usec = 0;
    sock = atm_net_connect(addr,port,&tov);
    if (sock != ATM_NET_ERR_FD) {
        res = atm_alloc(sizeof(atm_client_t));
        res->addr = addr;
        res->port = port;
        res->timeout = timeout;
        res->sockfd = sock;
    } else {
        atm_log_rout(ATM_LOG_ERROR, "can not get connect");
    }
    return res;
}


void
atm_client_send(atm_client_t *c, char *s)
{
    //TODO need implemented
}
