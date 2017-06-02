#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_

#include <atm_core.h>

#define ATM_CONFIG_BINDADDR_MAX 16

struct atm_config_s {
    /* General */
    pid_t pid;                          /* Main process pid. */
    atm_str_t configfile;               /* Absolute config file path, or NULL */
    atm_str_t executable;               /* Absolute executable file path. */
    atm_str_t *exec_argv;               /* Executable argv vector (copy). */
    atm_str_t pidfile;                  /* PID file path */
    atm_uint_t initial_memory_usage;    /* Bytes used after initialization. */
    atm_bool_t always_show_logo;        /* Show logo even for non-stdout logging. */
    /* Networking */
    int port;                           /* TCP listening port */
    int tcp_backlog;                    /* TCP listen() backlog */
    atm_str_t bindaddr[ATM_CONFIG_BINDADDR_MAX];
    int bindaddr_count;                 /* Number of addresses in server.bindaddr[] */
    int tcpkeepalive;                   /* Set SO_KEEPALIVE if non-zero. */
    /* Logging */
    atm_str_t logfile;                  /* Path of log file */
    atm_uint_t maxclients;              /* Max number of simultaneous clients */
    atm_ulong_t maxmemory;              /* Max number of memory bytes to use */
};

void
atm_config_init();

#endif /* _ATM_CONFIG_H_INCLUDED_ */
