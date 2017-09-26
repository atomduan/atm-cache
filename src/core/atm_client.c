#include <atm_core.h>
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */

void
atm_client_init()
{
    //prepare for epoll mechanism
}


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
    int sock = c->sockfd;
    int rem = strlen(s);
    char *buff = s;
    int count = 0;
    atm_log_rout(ATM_LOG_DEBUG, "atm_client_send enter");
    while (rem > 0) {
        count = write(sock, buff, rem);
        if (count > 0) {
            buff += count;
            rem -= count;
        } else {
            atm_log_rout(ATM_LOG_ERROR, "write error on send");
            break;
        }
    }
    atm_log_rout(ATM_LOG_DEBUG, "atm_client_send leave");
}


int
atm_client_recv(atm_client_t *c, char *buff, int size)
{
    int ret = 0;
    int rem = size;
    int total = 0;
    char *temp = buff;

    atm_log_rout(ATM_LOG_DEBUG, "atm_client_recv enter");
    while (ATM_TRUE) {
        ret = read(c->sockfd, temp, rem);
        if (ret > 0) {
            temp += ret;
            rem -= ret;
            total += ret;
        } else {
            if (ret < 0) {
                atm_log_rout(ATM_LOG_ERROR, "read error on recv");
                return ret;
            } else {
                /* can not read any more */
                break;
            }
        }
        if (rem == 0) break;
    }
    atm_log_rout(ATM_LOG_DEBUG, "atm_client_recv leave");
    return total;
}
