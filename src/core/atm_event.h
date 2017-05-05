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
    atm_bool_t  ep_rg;

    /* read active */
    atm_bool_t  r_act;
    /* write active */
    atm_bool_t  w_act;

    /* only be called by epoll */
    void (*handle_read)(atm_event_t  *ev);
    void (*handle_write)(atm_event_t *ev);
    void (*post_proc)(atm_event_t *ev);
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

/* 
 * 1. if e not managed by epoll then add it
 * 2. if the event and old mask then merge it
 * 3. mask the events's fd's bits in epoll
 * 4. activate it.
 */
void
atm_event_add_event(atm_event_t *e, int mask);

/*
 * 1. unmsk the event's fd's bits in epoll 
 * 2. if fd's bits is empty then del it from epoll
 *    and unactivate it.
 */
void
atm_event_del_event(atm_event_t *e, int unmask);


#endif /* _ATM_EVENT_H_INCLUDED_ */
