#ifndef _ATM_CONN_H_INCLUDED_
#define _ATM_CONN_H_INCLUDED_

#include <atm_core.h>


#define ATM_CONN_PERCALL_ACCEPTS    1000


struct atm_conn_s {
    atm_sess_t         *session;
    atm_socket_t       *sock;
    atm_event_t        *event;

    void (*handle_read)(atm_event_t *ev);
    void (*handle_write)(atm_event_t *ev);

    atm_buf_t   *r_buf;
    atm_buf_t   *w_buf;

    /* conn layer job controll */
    pthread_mutex_t    _mutex;
    atm_bool_t          on_read;
    atm_bool_t          on_write;
    atm_uint_t          write_reqs;
};

typedef struct {
    atm_socket_t       *ssck;
    atm_event_t        *event;

    void (*handle_accept)(atm_event_t *ev);
} atm_conn_listen_t;


void
atm_conn_init();

void
atm_conn_free(void *conn);

void
atm_conn_listen_free(void *listen);

atm_str_t *
atm_conn_read_line(atm_conn_t *c);

void
atm_conn_write(atm_conn_t *c, 
        void *src, atm_uint_t nbyte);

void
atm_conn_write_str(atm_conn_t *c, atm_str_t *s);


#endif /* _ATM_CONN_H_INCLUDED_ */
