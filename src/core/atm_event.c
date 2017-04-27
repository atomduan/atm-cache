#include <atm_core.h>


/*
 * Private 
 * */


static void
atm_event_process_ev();

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
atm_event_process_ev(atm_event_t *ev,uint32_t evs)
{

   if (evs & (EPOLLERR|EPOLLHUP)) {
       evs |= (EPOLLIN | EPOLLOUT);
   }

   if ((evs & EPOLLIN) && ev->active) {
       ev->rdy_read = ATM_TRUE;
       if (ev->handle_read != NULL) {
           ev->handle_read(ev);
       }
   }
   if ((evs & EPOLLOUT) && ev->active) {
       ev->rdy_write = ATM_TRUE;
       if (ev->handle_write != NULL) {
           ev->handle_write(ev);
       }
   }
}


static void
atm_event_process_events()
{
    int             events,i = 0;
    uint32_t        evs = 0; 
    atm_event_t    *ev = NULL;

    events = epoll_wait(ep,event_list,
            (int) nevents,ATM_EVENT_BLOCK);
    
    if (events > 0) {
       for (i=0; i<events; ++i) {
           /* TODO thunder herd problem solved? */
           ev = event_list[i].data.ptr;
           if (ev->fd == -1) {
               continue; 
           }
           evs = event_list[i].events;
           atm_event_process_ev(ev, evs);
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


/* get new event instance*/
atm_event_t *
atm_event_new(void *load, int fd, 
        void (*handle_read)(void *ev),
        void (*handle_write)(void *ev))
{
    atm_event_t *res = NULL;
    res = atm_alloc(sizeof(atm_event_t));
    res->fd = fd;
    res->active = ATM_FALSE;
    res->load = load;
    res->rdy_read = ATM_FALSE;
    res->rdy_write = ATM_FALSE;
    res->handle_read = handle_read;
    res->handle_write = handle_write;
    return res;
}


void
atm_event_free(void *e)
{
    atm_free(e);
}


/* public funcs */
void
atm_event_routine()
{
    atm_event_process_events();
}


void
atm_event_add_listen(atm_listen_t *l)
{
    struct epoll_event ee;
    atm_event_t *le = NULL;
    int sfd = 0;

    if (l != NULL) {
        sfd = l->ssck->fd;
        le = atm_event_new(l,sfd,l->handle_accept,NULL); 

        ee.events = EPOLLIN|EPOLLHUP;
        ee.data.ptr = le;
        epoll_ctl(ep, EPOLL_CTL_ADD, sfd, &ee);
        le->active = ATM_TRUE;
        l->event = le;
    }
}


void
atm_event_add_conn(atm_conn_t *c)
{
    struct epoll_event ee;
    atm_event_t *ce = NULL;
    int cfd = 0;

    if (c != NULL) {
        cfd = c->sock->fd;
        ce = atm_event_new(c, cfd, 
                c->handle_read, 
                c->handle_write); 

        ee.events = EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLET;
        ee.data.ptr = ce;
        epoll_ctl(ep, EPOLL_CTL_ADD, cfd, &ee);
        ce->active = ATM_TRUE;
        c->event = ce;
    }
}


void
atm_event_del_conn(atm_conn_t *c)
{
    struct epoll_event ee;
    atm_event_t *ce = NULL;
    int cfd = 0;

    if (c != NULL) {
        cfd = c->sock->fd;
        ce = c->event;
        ee.events = 0;
        ee.data.ptr = NULL;
        epoll_ctl(ep, EPOLL_CTL_DEL, cfd, &ee);
        ce->active = ATM_FALSE;
    }
}
