#ifndef _ATM_CONN_H_INCLUDED_
#define _ATM_CONN_H_INCLUDED_

#include <atm_core.h>


#define ATM_CONN_PERCALL_ACCEPTS    1000


struct atm_conn_listen_s {
    atm_socket_t       *ssck;
    atm_event_t        *event;

    void (*handle_accept)(atm_event_t *ev);
    void (*post_proc)(atm_event_t *ev);
};


struct atm_conn_s {
    atm_sess_t         *session;
    atm_socket_t       *sock;
    atm_event_t        *event;

    void (*handle_read)(atm_event_t *ev);
    void (*handle_write)(atm_event_t *ev);
    void (*post_proc)(atm_event_t *ev);
};


void
atm_conn_init();

void
atm_conn_free(void *conn);

void
atm_conn_wnotify(void *conn);


#endif /* _ATM_CONN_H_INCLUDED_ */
