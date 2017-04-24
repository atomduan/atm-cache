#ifndef _ATM_EVENT_H_INCLUDED_
#define _ATM_EVENT_H_INCLUDED_

#include <atm_core.h>


struct atm_event_s {

};


struct atm_event_actions_s {
    atm_int_t  (*add)(atm_event_t *ev);
    atm_int_t  (*del)(atm_event_t *ev);

    atm_int_t  (*enable)(atm_event_t *ev);
    atm_int_t  (*disable)(atm_event_t *ev);

    atm_int_t  (*add_conn)(atm_conn_t *conn);
    atm_int_t  (*del_conn)(atm_conn_t *conn);

    atm_int_t  (*poll_events)(atm_ctx_t *ctx, atm_uint_t timer);

    atm_int_t  (*init)(atm_ctx_t *ctx, atm_uint_t timer);
    void       (*done)(atm_ctx_t *ctx);
};


void
atm_event_init();

void
atm_event_rout(atm_ctx_t *atm_ctx);


#endif /* _ATM_EVENT_H_INCLUDED_ */
