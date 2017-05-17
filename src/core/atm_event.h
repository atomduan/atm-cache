#ifndef _ATM_EVENT_H_INCLUDED_
#define _ATM_EVENT_H_INCLUDED_

#include <atm_core.h>


#define ATM_EVENT_BLOCK        -1   
#define ATM_EVENT_NONE          0
#define ATM_EVENT_ALL          -1

#define ATM_EVENT_SIZE          1024   
#define ATM_EVENT_LIST_SIZE     1024   

#define ATM_EVENT_READ          EPOLLIN   
#define ATM_EVENT_WRITE         EPOLLOUT 


struct atm_event_s {
    int         fd;
    uint32_t    events;

    /* atm_conn_t, atm_listen_t, etc */
    void       *load;

    /* wether register in epoll*/
    atm_bool_t  active;

    /* only be called by epoll */
    void (*handle_read)(atm_event_t *ev);
    void (*handle_write)(atm_event_t *ev);
    void (*post_proc)(atm_event_t *ev);

    /* job controll */
    pthread_mutex_t     mutex;
    atm_bool_t          on_read;
    atm_uint_t          read_event_count;
    atm_bool_t          on_write;
    atm_uint_t          write_event_count;
};


/* public event type lifecycle */
void
atm_event_init();

/* get new event instance*/
atm_event_t *
atm_event_new(void *load, int fd, 
        void (*handle_read)(atm_event_t *ev),
        void (*handle_write)(atm_event_t *ev),
        void (*post_proc)(atm_event_t *ev));

void
atm_event_free(void *e);

/* public funcs */
void
atm_event_routine();

void
atm_event_add_listen(atm_conn_listen_t *l);

void
atm_event_add_conn(atm_conn_t *e);

void
atm_event_add_event(atm_event_t *e, int mask);

void
atm_event_del_event(atm_event_t *e, int unmask);

atm_bool_t
atm_event_yield_read(
        atm_event_t *e, atm_uint_t last_count);

atm_bool_t
atm_event_yield_write(
        atm_event_t *e, atm_uint_t last_count);


#endif /* _ATM_EVENT_H_INCLUDED_ */
