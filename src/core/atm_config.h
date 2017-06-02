#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_

#include <atm_core.h>

#define ATM_CONFIG_MAXBINDADDR 16
#define ATM_CONFIG_MAXLINE 1024

struct atm_config_s {
    atm_str_t   configfile;     /* Absolute config file path, or NULL */
    atm_str_t   pidfile;        /* PID file path */
    atm_int_t   daemonize;

    /* Networking */
    atm_str_t  *bindaddr;
    atm_uint_t  bindaddr_count; /* Number of addresses in server.bindaddr[] */
    atm_int_t   port;           /* TCP listening port */
    atm_int_t   tcpbacklog;     /* TCP listen() backlog */
    atm_int_t   maxidletime;
    atm_int_t   tcpkeepalive;   /* Set SO_KEEPALIVE if non-zero. */
    atm_int_t   maxclients;     /* Max number of simultaneous clients */

    /* Logging */
    atm_int_t   loglevel;
    /* Memory */
    atm_ulong_t maxmemory;      /* Max number of memory bytes to use */
    /* Tasks */
    atm_int_t   workernum;
};

void
atm_config_init();

#endif /* _ATM_CONFIG_H_INCLUDED_ */
