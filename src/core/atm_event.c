#include <atm_core.h>


/*
 * Private 
 * */


static void
atm_event_process_events();


static int                   ep = -1;
static struct epoll_event   *event_list = NULL;
static atm_uint_t            nevents = 0;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */


static void
atm_event_process_events()
{
    int             events,i = 0;
    uint32_t        est = 0; 
    atm_conn_t     *conn = NULL;
    atm_event_t    *rev = NULL;
    atm_event_t    *wev = NULL;

    events = epoll_wait(ep,event_list,
            (int) nevents,ATM_EVENT_NOBLK);
    
    if (events > 0) {
       for (i=0; i<events; ++i) {
           conn = event_list[i].data.ptr;
           if (conn->fd == -1) {
               continue; 
           }
           est = event_list[i].events;
           if (est & (EPOLLERR|EPOLLHUP)) {
               est |= (EPOLLIN | EPOLLOUT);
           }
           if ((est & EPOLLIN) && rev->active) {
               rev = conn->read;
               rev->ready = ATM_TRUE;
               rev->handle(rev);
           }
           if ((est & EPOLLOUT) && wev->active) {
               wev = conn->write;
               wev->ready = ATM_TRUE;
               wev->handle(wev);
           }
       }
    }
}


/*
 * Public
 * */
void
atm_event_init()
{
    /* set up event engine. */
    ep = epoll_create(ATM_EVENT_SIZE);
    if (event_list) {
        atm_free(event_list);
    }
    nevents = ATM_EVENT_LIST_SIZE;
    event_list = atm_alloc(
        sizeof(struct epoll_event) * nevents);
}


/* public funcs */
void
atm_event_routine()
{
    atm_event_process_events();
}


void
atm_event_add_conn(atm_conn_t *c)
{
    struct epoll_event ee;

    if (c != NULL) {
        ee.events = EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLET;
        ee.data.ptr = c;
        epoll_ctl(ep, EPOLL_CTL_ADD, c->fd, &ee);
        c->read->active = ATM_TRUE;
        c->write->active = ATM_TRUE;
    }
}


void
atm_event_del_conn(atm_conn_t *c)
{
    struct epoll_event ee;

    if (c != NULL) {
        ee.events = 0;
        ee.data.ptr = NULL;
        epoll_ctl(ep, EPOLL_CTL_DEL, c->fd, &ee);
        c->read->active = ATM_FALSE;
        c->write->active = ATM_FALSE;
    }
}
