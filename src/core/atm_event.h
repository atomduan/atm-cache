#ifndef _ATM_EVENT_H_INCLUDED_
#define _ATM_EVENT_H_INCLUDED_

#include <atm_core.h>


#define ATM_EVENT_NOBLK         0   
#define ATM_EVENT_SIZE          1024   
#define ATM_EVENT_LIST_SIZE     1024   


struct atm_event_s {
    void       *data;
    void      (*handle)(atm_event_t *ev);
    atm_bool_t  active;
    atm_bool_t  ready;
};


/* public event type lifecycle */
void
atm_event_init();


/* public funcs */
void
atm_event_routine();

void
atm_event_add_conn(atm_conn_t *e);

void
atm_event_del_conn(atm_conn_t *e);


#endif /* _ATM_EVENT_H_INCLUDED_ */
