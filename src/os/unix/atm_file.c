#include <atm_core.h>
/*
 * Private
 * */
static void
atm_pip_handle_recv(atm_event_t *e);
static void 
atm_pipe_process_msgs(atm_pipe_t *pipe, char *notify);
static atm_pipe_msg_t *
atm_pipe_msg_new(void *load, void (*call_back)(void *load));
static void
atm_pipe_msg_free(void *msg);

static atm_T_t ATM_PIPE_MSG_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_pipe_msg_free,
};

static atm_T_t *ATM_PIPE_MSG_T = &ATM_PIPE_MSG_TYPE;

/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_pip_handle_recv(atm_event_t *e)
{
    int processed;
    int ret;
    char buf[1];
    int recv_fd = e->fd;
    atm_pipe_t *p = e->load;

    processed = 0;
    pthread_mutex_lock(&p->mqlock);
    while (ATM_TRUE) {
        ret = read(recv_fd, buf, 1);
        if (ret > 0) {
            if (!processed) {
                atm_pipe_process_msgs(p,buf);             
                processed++;
            }
            /* flush all buf */
            continue;
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&p->mqlock);
}


static void 
atm_pipe_process_msgs(atm_pipe_t *pipe, 
        char *notify)
{
    atm_pipe_t *p = pipe;
    atm_pipe_msg_t *msg;
    switch (notify[0]) {
        case ATM_PIPE_NCHAR:
            while (ATM_TRUE) {
                msg=atm_queue_pop(p->mqueue);
                if (msg == NULL) break;
                atm_log("pipe_process msg[%p],[%p][%p]",msg,msg->load,msg->call_back);
                /* whether to dispatch is msg's biz */
                msg->call_back(msg->load);
                atm_log("get msg instance from pipe notfy");
                atm_pipe_msg_free(msg);
            }
        break;
    }
}


static atm_pipe_msg_t *
atm_pipe_msg_new(void *load,
        void (*call_back)(void *load))
{
    atm_pipe_msg_t *res = NULL;
    res = atm_alloc(sizeof(atm_pipe_msg_t));
    res->load = load;
    res->call_back = call_back;
    return res;
}


static void
atm_pipe_msg_free(void *msg)
{
    atm_pipe_msg_t *m = msg;
    if (m != NULL) {
        atm_free(m);
    }
}


/*
 * Public
 * */
atm_int_t
atm_file_nonblock(int fd, 
        atm_bool_t nblk)
{
    int flags;
    /* 
     * Note that fcntl(2) for F_GETFL 
     * and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags=fcntl(fd, F_GETFL))==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_GETFL): %s", 
            strerror(errno));
        return ATM_ERROR;
    }

    if (nblk)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd,F_SETFL,flags)==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_SETFL,O_NONBLOCK): %s", 
            strerror(errno));
        return ATM_ERROR;
    }
    return ATM_OK;
}


atm_pipe_t *
atm_pipe_new()
{
    atm_pipe_t *res = NULL;
    int fds[2];

    if (pipe(fds)) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not create notify pipe");
        exit(1);
    }

    res = atm_alloc(sizeof(atm_pipe_t));
    res->recv_fd = fds[0];
    atm_file_nonblock(res->recv_fd,ATM_TRUE);
    res->sent_fd = fds[1];
    atm_file_nonblock(res->sent_fd,ATM_TRUE);

    res->mqueue = atm_queue_new(
                ATM_PIPE_MSG_T,
                ATM_FREE_DEEP,
                ATM_QUEUE_NONBLOCK);

    pthread_mutex_init(&res->mqlock, NULL);
    return res;
}


/*
 * trust event process to epoll
 */
void
atm_pipe_event_init(atm_pipe_t *pipe)
{
    atm_event_t *pe;
    int events = ATM_EVENT_NONE;

    pe = atm_event_new(
            pipe,
            pipe->recv_fd,
            atm_pip_handle_recv,
            NULL); 

    events = EPOLLIN|EPOLLHUP|EPOLLET;
    atm_event_add_event(pe, events);
    pipe->event = pe;
}


void
atm_pipe_free(void *pipe)
{
    /*
     * TODO the msg in pip 
     * SHOULD NOT be * discard !!!
     * we need another queue to place it
     * or wait it to finish
     */
    atm_pipe_t *p = pipe;
    if (p != NULL) {
        if (p->event != NULL) 
            atm_event_free(p->event); 
        atm_queue_free(p->mqueue);
        pthread_mutex_destroy(&p->mqlock);
        atm_free(p);
    }
}


void
atm_pipe_notify(atm_pipe_t *pipe, void *load, 
        void (*call_back)(void *load))
{
    char buf[1];
    atm_pipe_t *p;
    atm_pipe_msg_t *msg; 

    buf[0] = ATM_PIPE_NCHAR;
    p = pipe;
    msg = atm_pipe_msg_new(load,call_back);
    
    atm_log("pipe_notify msg[%p],[%p][%p]",msg,msg->load,msg->call_back);
    pthread_mutex_lock(&p->mqlock);
    atm_queue_push(p->mqueue, msg);
    pthread_mutex_unlock(&p->mqlock);
    
    /* sent pipe notify */
    if (write(p->sent_fd,buf,1)!=1) {
        atm_log_rout(ATM_LOG_FATAL, 
                "pipe notify fail");
    }
}
