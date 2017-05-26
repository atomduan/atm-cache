#ifndef _ATM_SOCKET_H_INCLUDED_
#define _ATM_SOCKET_H_INCLUDED_

#include <atm_core.h>

#define ATM_NET_ERR_FD -1
#define ATM_NET_IPSLEN 46
#define ATM_NET_PORTSLEN 6
#define ATM_NET_DEFAULT_TCP_KEEPALIVE 300


struct atm_socket_s {
    int         fd;
    atm_str_t   src_ip; 
    int         src_port;
};


void
atm_socket_free(void *s);

atm_socket_t *
atm_net_listen_tcp(int port, 
        char *bindaddr, int backlog);

atm_socket_t *
atm_net_accept(atm_socket_t *ss);

int       
atm_net_nonblock(atm_socket_t *s, 
        atm_bool_t non_block); 

int       
atm_net_nodelay(atm_socket_t *s, 
        atm_bool_t nodelay); 

int       
atm_net_keepalive(atm_socket_t *s, 
        int interval); 


#endif /* _ATM_SOCKET_H_INCLUDED_ */
