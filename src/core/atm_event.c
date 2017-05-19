#include <atm_core.h>
/*
 * Private 
 * */
static void 
atm_event_notify_recv(atm_event_t *ev);

static void
atm_event_nonblock_fd(int fd);

static void
atm_event_pipe_init();

static void
atm_event_process_ev();

static void
atm_event_process_events();


static int                   ep = -1;
static struct epoll_event   *event_list = NULL;
static atm_uint_t            nevents = 0;


/* pipe define */
static int pipe_recv_fd = -1;
static int pipe_sent_fd = -1;


typedef struct {
    atm_event_t *e;
    int          mask;
} atm_event_msg_t;

static atm_event_msg_t *
atm_event_msg_new(atm_event_t *e, int mask);

static void
atm_event_msg_free(void *e);


static atm_list_t *event_msgs;
static pthread_mutex_t event_msgs_lk = PTHREAD_MUTEX_INITIALIZER;
static atm_T_t ATM_EVENT_MSG_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_event_msg_free,
};
static atm_T_t *ATM_EVENT_MSG_T = &ATM_EVENT_MSG_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_event_msg_t *
atm_event_msg_new(atm_event_t *e, int mask)
{
    atm_event_msg_t *res = NULL;
    res = atm_alloc(sizeof(atm_event_msg_t));
    res->e = e;
    res->mask = mask;
    return res;
}


static void
atm_event_msg_free(void *e)
{
    if (e != NULL) {
        atm_free(e);
    }
}


static void 
atm_event_process_tasks(char *notify)
{
    atm_event_msg_t *m = NULL;
    switch (notify[0]) {
        case 'A':
            /* this is a single thread impl */
            while (ATM_TRUE) {
                m=atm_list_lpop(event_msgs);
                if (m == NULL) break;
                atm_log("add event from notify");
                atm_event_add_event(m->e, m->mask);
            }
        break;
    }
}


static void 
atm_event_notify_recv(atm_event_t *e)
{
    int processed = 0;
    int ret = 0;
    char buf[1];
    int pipe_rfd = e->fd;

    atm_log("atm_event_notify_recv enter");
    pthread_mutex_lock(&event_msgs_lk);
    while (ATM_TRUE) {
        ret = read(pipe_rfd, buf, 1);
        if (ret > 0) {
            if (!processed) {
                atm_event_process_tasks(buf);             
                processed++;
            }
            continue;
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&event_msgs_lk);
}


static void
atm_event_nonblock_fd(int fd)
{
    int flags;
    if ((flags=fcntl(fd, F_GETFL))==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_GETFL): %s", 
            strerror(errno));
        exit(1);
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd,F_SETFL,flags)==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_SETFL,O_NONBLOCK): %s", 
            strerror(errno));
        exit(1);
    }
}


static void
atm_event_pipe_init()
{
    int fds[2];
    atm_event_t *pe = NULL;
    uint32_t events = ATM_EVENT_NONE;

    if (pipe(fds)) {
        atm_log_rout(ATM_LOG_FATAL, 
            "can not create event notify pipe");
        exit(1);
    }

    pipe_recv_fd = fds[0];
    atm_event_nonblock_fd(pipe_recv_fd);
    pipe_sent_fd = fds[1];
    atm_event_nonblock_fd(pipe_sent_fd);

    pe = atm_event_new(
            NULL,
            pipe_recv_fd,
            atm_event_notify_recv,
            NULL); 

    events = EPOLLIN|EPOLLHUP|EPOLLET;
    atm_log("event initial events mask %u", events);
    atm_event_add_event(pe, events);
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
    int             ecnt,i = 0;
    uint32_t        evs = 0; 
    atm_event_t    *ev = NULL;

    ecnt = epoll_wait(ep,event_list,
            (int) nevents,ATM_EVENT_BLOCK);
    
    if (ecnt > 0) {
       for (i=0; i<ecnt; ++i) {
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

    event_msgs = atm_list_new(
            ATM_EVENT_MSG_T,
            ATM_FREE_DEEP);
    atm_event_pipe_init();
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
    res->registed = ATM_FALSE;
    res->active = ATM_TRUE;
    res->handle_read = handle_read;
    res->handle_write = handle_write;

    pthread_mutex_init(&res->mutex, NULL);
    res->on_write = ATM_FALSE;
    res->write_reqs = 0;

    return res;
}


void
atm_event_free(void *ev)
{
    atm_event_t *e = ev;
    if (e != NULL) {
        atm_event_del_event(e, ATM_EVENT_ALL);
        pthread_mutex_destroy(&e->mutex);
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
    int sfd = -1;
    atm_event_t *le = NULL;
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
    int cfd = -1;
    atm_event_t *ce = NULL;
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
    int fd =-1;
    int op = 0;
    int ret = 0;
    struct epoll_event ee;

    atm_log("add event mask is %d",mask);
    if (e != NULL) {
        fd = e->fd;
        e->events = e->events | mask;
        if (e->events != ATM_EVENT_NONE) {
            if (e->registed) {
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
            e->registed = ATM_TRUE;
        }
    }
}


void
atm_event_add_notify(atm_event_t *e, uint32_t mask)
{
    atm_event_msg_t *m = NULL;
    char buf[1];

    buf[0] = 'A';
    if (e != NULL) {
        pthread_mutex_lock(&event_msgs_lk);
        m = atm_event_msg_new(e, mask);
        atm_list_push(event_msgs, m);
        if (write(pipe_sent_fd,buf,1)!=1) {
            atm_log_rout(ATM_LOG_ERROR, 
                "event notify to pipe fail");
        }
        pthread_mutex_unlock(&event_msgs_lk);
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
    int fd =-1;
    int op = 0;
    int ret = -1;
    struct epoll_event ee;

    atm_log("del event unmask is %u",unmask);
    if (e != NULL) {
        fd = e->fd;
        e->events = e->events & (~unmask);
        if (e->registed) {
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
                e->registed = ATM_FALSE;
            }
        }
    }
}


void
atm_event_inter_write(atm_event_t *e, atm_uint_t wreqs)
{
    pthread_mutex_lock(&e->mutex);
    e->on_write = ATM_FALSE;
    if(e->write_reqs > wreqs) {
        if (e->active) {
            atm_event_add_notify(e, ATM_EVENT_WRITE);
        }
    }
    pthread_mutex_unlock(&e->mutex);
}


atm_bool_t
atm_event_yield_write(atm_event_t *e, atm_uint_t wreqs)
{
    atm_bool_t res = ATM_FALSE;
    pthread_mutex_lock(&e->mutex);
    if(e->write_reqs == wreqs || !e->active) {
        e->on_write = ATM_FALSE;
        res = ATM_TRUE;
    }
    pthread_mutex_unlock(&e->mutex);
    return res;
}


void
atm_event_write_notify(atm_event_t *e)
{
    pthread_mutex_lock(&e->mutex);
    if (e->active) {
        e->write_reqs += 1;
        if (e->on_write == ATM_FALSE) {
            atm_log("atm_event_write_notify add");
            atm_event_add_notify(e, ATM_EVENT_WRITE); 
        } else {
            atm_log("atm_event_wirte_notify req");
        }
    }
    pthread_mutex_unlock(&e->mutex);
}


void
atm_event_inactive(atm_event_t *e)
{
    pthread_mutex_lock(&e->mutex);
    e->active = ATM_FALSE;
    pthread_mutex_unlock(&e->mutex);
}
