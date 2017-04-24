#include <atm_core.h>


/* ---------------------IMPLEMENTATIONS--------------------------- */


/*
 * Public
 * */


void
atm_event_init()
{
    atm_epoll_init();
}


void
atm_event_rout(atm_ctx_t *atm_ctx)
{
    actions->poll_events();
}
