#ifndef _ATM_SOCKET_H_INCLUDED_
#define _ATM_SOCKET_H_INCLUDED_

#include <atm_core.h>

#define ATM_NET_ERR_FD -1

struct atm_socket_s {
    int     fd;
};


atm_socket_t *
atm_net_listen_tcp(int port, 
        char *bindaddr, int backlog);

int       
atm_net_nonblock(atm_socket_t *s, 
        atm_bool_t non_block); 

#endif /* _ATM_SOCKET_H_INCLUDED_ */
