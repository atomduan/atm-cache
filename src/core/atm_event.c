#include <atm_core.h>
/*
 * Private 
 * */
static void
atm_event_notify_handle(void *eop);
static atm_event_op_t *
atm_event_op_new(atm_event_t *e, uint32_t mask, atm_uint_t op_type);
static void
atm_event_op_free(void *eop);
static void
atm_event_process_ev();
static void
atm_event_process_events();


/* epoll define */
static int                   ep = -1;
static struct epoll_event   *event_list = NULL;
static atm_uint_t            nevents = 0;


/* pipe define */
static atm_pipe_t *notify_pipe;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_event_notify_handle(void *eop)
{
    atm_event_op_t *op = eop;
    if (op != NULL) {
        if (op->op_type == ATM_EVENT_OP_ADD) {
            atm_event_add_event(op->e, op->mask);
        } else
        if (op->op_type == ATM_EVENT_OP_DEL) {
            atm_event_del_event(op->e, op->mask);
        }
    }
    atm_event_op_free(eop);
}


static atm_event_op_t *
atm_event_op_new(atm_event_t *e, 
        uint32_t mask, atm_uint_t op_type)
{
    atm_event_op_t *res = NULL;
    res = atm_alloc(sizeof(atm_event_op_t));
    res->e = e;
    res->mask = mask;
    res->op_type = op_type;
    return res;
}


static void
atm_event_op_free(void *eop)
{
    atm_event_op_t *op = eop;
    if (op != NULL) {
        atm_free(op);
    }
}


static void
atm_event_process_ev(atm_event_t *ev, uint32_t evs)
{
    if (evs & (EPOLLERR|EPOLLHUP)) {
        evs |= (EPOLLIN | EPOLLOUT);
    }
    if (evs & EPOLLIN) {
        if (ev->handle_read != NULL) {
            if (ev->active) {
                ev->handle_read(ev);
            }
        }
    }
    if ((evs & EPOLLOUT)) {
        if (ev->handle_write != NULL) {
            if (ev->active) {
                ev->handle_write(ev);
            }
        }
    }
}


static void
atm_event_process_events()
{
    int             ev_count,i;
    uint32_t        evs; 
    atm_event_t    *ev;

    ev_count = epoll_wait(ep,event_list,
            (int) nevents,ATM_EVENT_BLOCK);
    
    if (ev_count > 0) {
       for (i=0; i<ev_count; ++i) {
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
    /* discard old data */
    if (event_list) atm_free(event_list);
    nevents = ATM_EVENT_LIST_SIZE;
    event_list = atm_alloc(
        sizeof(struct epoll_event) * nevents);

    /* dispatch pipe init*/
    notify_pipe = atm_pipe_new();
    /* trust pipe event manage to epoll */
    atm_pipe_event_init(notify_pipe);
}


/* get new event instance*/
atm_event_t *
atm_event_new(void *load, int fd, 
        void (*handle_read)(atm_event_t *ev),
        void (*handle_write)(atm_event_t *ev))
{
    atm_event_t *res = NULL;
    res = atm_alloc(sizeof(atm_event_t));
    res->fd = fd;
    res->events = ATM_EVENT_NONE;
    res->load = load;
    res->_registed = ATM_FALSE;
    res->active = ATM_TRUE;
    res->handle_read = handle_read;
    res->handle_write = handle_write;
    return res;
}


void
atm_event_free(void *ev)
{
    atm_event_t *e = ev;
    if (e != NULL) {
        atm_event_del_event(e, ATM_EVENT_ALL);
        atm_free(e);
    }
}


/* public funcs */
void
atm_event_routine()
{
    atm_event_process_events();
}


void
atm_event_add_listen(atm_conn_listen_t *l)
{
    int sfd;
    atm_event_t *le;
    uint32_t events = ATM_EVENT_NONE;

    if (l != NULL) {
        sfd = l->ssck->fd;
        le = l->event;
        if (le == NULL) {
            le = atm_event_new(l,sfd,
                l->handle_accept,
                NULL); 
            l->event = le;
        }
        events = EPOLLIN|EPOLLHUP;
        atm_event_add_event(le, events);
    }
}


void
atm_event_add_conn(atm_conn_t *c)
{
    int cfd;
    atm_event_t *ce;
    uint32_t events = ATM_EVENT_NONE;

    if (c != NULL) {
        cfd = c->sock->fd;
        ce = c->event;
        if (ce == NULL) {
            ce = atm_event_new(c, cfd, 
                    c->handle_read, 
                    c->handle_write); 
            c->event = ce;
        }

        events = EPOLLIN|EPOLLHUP|EPOLLET;
        atm_log("conn initial events mask %u", events);
        atm_event_add_event(ce, events);
    }
}


/* 
 * 1. if e not managed by epoll then add it
 * 2. if the event and old mask then merge it
 * 3. mask the events's fd's bits in epoll
 * 4. activate it.
 */
void
atm_event_add_event(atm_event_t *e, uint32_t mask)
{
    int fd;
    int op;
    int ret;
    struct epoll_event ee;

    atm_log("add event mask is %u",mask);
    if (e != NULL) {
        fd = e->fd;
        e->events = e->events | mask;
        if (e->events != ATM_EVENT_NONE) {
            if (e->_registed) {
                op = EPOLL_CTL_MOD;
                atm_log("add event op is mod");
            } else {
                op = EPOLL_CTL_ADD;
                atm_log("add event op is add");
            }
            ee.events = e->events;
            ee.data.ptr = e;
            while (ATM_TRUE) {
                ret = epoll_ctl(ep, op, fd, &ee);
                if (ret == -1) {
                    atm_log("errno is %d", errno);
                }
                break;
            }
            atm_log("epoll_ctl add ret is %d", ret);
            e->_registed = ATM_TRUE;
        }
    }
}


/*
 * 1. unmsk the event's fd's bits in epoll 
 * 2. if fd's bits is empty then del it from epoll
 *    and unactivate it.
 */
void
atm_event_del_event(atm_event_t *e, uint32_t unmask)
{
    int fd;
    int op;
    int ret;
    struct epoll_event ee;

    atm_log("del event unmask is %u",unmask);
    if (e != NULL) {
        fd = e->fd;
        e->events = e->events & (~unmask);
        if (e->_registed) {
            if (e->events != ATM_EVENT_NONE) {
                op = EPOLL_CTL_MOD;
                atm_log("del event op is mod");
            } else {
                op = EPOLL_CTL_DEL;
                atm_log("del event op is del");
            }
            ee.events = e->events;
            ee.data.ptr = e;

            while (ATM_TRUE) {
                ret = epoll_ctl(ep, op, fd, &ee);
                if (ret == -1){
                    atm_log("errno is %d", errno);
                }
                break;
            }
            atm_log("epoll_ctl del ret is %d", ret);

            if (op == EPOLL_CTL_DEL) {
                e->_registed = ATM_FALSE;
            }
        }
    }
}


void
atm_event_inactive(atm_event_t *e)
{
    e->active = ATM_FALSE;
}


void
atm_event_add_notify(atm_event_t *e, uint32_t mask)
{
    atm_uint_t type = ATM_EVENT_OP_ADD; 
    atm_event_op_t *eop = atm_event_op_new(e,mask,type);

    atm_pipe_notify(
            notify_pipe,
            eop,
            atm_event_notify_handle);
}
