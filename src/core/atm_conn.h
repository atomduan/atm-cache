#ifndef _ATM_CONN_H_INCLUDED_
#define _ATM_CONN_H_INCLUDED_

#include <atm_core.h>


#define ATM_CONN_PERCALL_ACCEPTS    1000


struct atm_conn_listen_s {
    atm_socket_t       *ssck;
    atm_event_t        *event;
    void (*handle_accept)(atm_event_t *ev);
};


struct atm_conn_s {
    atm_session_t      *session;
    atm_socket_t       *sock;
    atm_event_t        *event;
    void (*handle_read)(atm_event_t *ev);
    void (*handle_write)(atm_event_t *ev);
};


void
atm_conn_init();

void
atm_conn_free(void *conn);

void
atm_conn_listen_free(void *listen);

void
atm_conn_handle_accept(atm_event_t *listen_event);

void
atm_conn_handle_read(atm_event_t *conn_event);

void
atm_conn_handle_write(atm_event_t *conn_event);


#endif /* _ATM_CONN_H_INCLUDED_ */
