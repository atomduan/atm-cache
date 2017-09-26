#ifndef _ATM_CLIENT_H_INCLUDED_
#define _ATM_CLIENT_H_INCLUDED_

#include <atm_core.h>

struct atm_client_s {
    char   *addr;
    int     port;
    int     timeout;
    int     sockfd;
};


void
atm_client_init();

atm_client_t *
atm_client_new(char *addr, int port, int timeout);

void
atm_client_send(atm_client_t *c, char *s);

int
atm_client_recv(atm_client_t *c, char *buff, int size);

#endif /* _ATM_CLIENT_H_INCLUDED_ */

