#include <atm_core.h>
/*
 * Private
 * */
static void
atm_task_worker_dispatch(atm_task_t *t); 

static void *
atm_task_worker_func(void *arg);

static atm_task_t *
atm_task_worker_blocking_wait(
        atm_task_worker_t *worker, atm_uint_t timeout);

static void
atm_task_worker_init(int nworker);

static atm_task_worker_t *
atm_task_worker_new();

static void 
atm_task_worker_free(void *worker);

static void 
atm_task_notify();

static void 
atm_task_process_tasks(char *notify);

static void 
atm_task_notify_recv(atm_event_t *ev);

static void
atm_task_nonblock_fd(int fd);

static void
atm_task_pipe_init();


/* pipe define */
static int pipe_recv_fd = -1;
static int pipe_sent_fd = -1;


/* tasks define */
static atm_list_t *tasks;
static atm_T_t ATM_TASK_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_task_free,
};
static atm_T_t *ATM_TASK_T = &ATM_TASK_TYPE;


/* worker define */
static atm_list_t *workers;
static atm_T_t ATM_TASK_WORKER_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_task_worker_free,
};
static atm_T_t *ATM_TASK_WORKER_T = &ATM_TASK_WORKER_TYPE;


static pthread_mutex_t workers_lk = PTHREAD_MUTEX_INITIALIZER;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_task_worker_dispatch(atm_task_t *t)
{
    atm_list_t  *wts = NULL;
    atm_task_worker_t *curr_worker = NULL;

    pthread_mutex_lock(&workers_lk);
    curr_worker = atm_list_round(workers);
    pthread_mutex_unlock(&workers_lk);

    if (curr_worker == NULL) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not get curr_worker");
        exit(1);
    }

    pthread_mutex_lock(&curr_worker->qlock);
    wts = curr_worker->wtasks;
    atm_list_push(wts, t);
    pthread_mutex_unlock(&curr_worker->qlock);
    /* TODO how dose qlock work? */
    pthread_cond_signal(&curr_worker->qready);
}


static void *
atm_task_worker_func(void *arg)
{
    void *res = NULL;
    atm_task_worker_t *worker = NULL;
    atm_list_t *wtasks = NULL;
    atm_task_t *t = NULL;

    worker = arg;
    wtasks = worker->wtasks;
    while (worker->active) {
        t = atm_task_worker_blocking_wait(worker, 
                ATM_TASK_WORKER_BLOCKING_INTVAL);
        if (t != NULL) {
            t->retry = ATM_FALSE;
            t->run(t);
            if (t->retry == ATM_TRUE) {
                atm_task_worker_dispatch(t);
            } else {
                atm_task_free(t);
            }
        }
    }
    return res;
}


/* The time out metric is second */
static atm_task_t *
atm_task_worker_blocking_wait(
        atm_task_worker_t *worker, atm_uint_t timeout)
{
    atm_task_t *res = NULL;
    atm_list_t *wtasks = NULL;

    struct timespec ts;
    struct timeval now;

    gettimeofday(&now, NULL);
    ts.tv_sec = now.tv_sec;
    ts.tv_sec += timeout;

    wtasks = worker->wtasks;
    pthread_mutex_lock(&worker->qlock);
    while (wtasks->size == 0) {
        /* TODO how does qlock work with it? */
        pthread_cond_timedwait(&worker->qready, 
                &worker->qlock,&ts);
    }
    res = atm_list_lpop(wtasks);
    pthread_mutex_unlock(&worker->qlock);

    return res;
}


static void
atm_task_worker_init(int nworker)
{
    atm_task_worker_t *w = NULL;
    workers = atm_list_new(
            ATM_TASK_WORKER_T, 
            ATM_FREE_DEEP);
    for (int i=0; i<nworker; ++i) {
        w = atm_task_worker_new();
        if (w != NULL) {
            atm_list_push(workers, w);
        }
    }
}


static atm_task_worker_t *
atm_task_worker_new()
{
    atm_task_worker_t *res = NULL;
    atm_list_t *wtasks = NULL;

    int ret;
    pthread_t tid;
    pthread_attr_t attr;

    wtasks = atm_list_new(
                ATM_TASK_T,
                ATM_FREE_DEEP);
    res = atm_alloc(sizeof(atm_task_worker_t));
    res->wtasks = wtasks;
    res->active = ATM_TRUE;
    res->tid = 0;
    pthread_cond_init(&res->qready, NULL);
    pthread_mutex_init(&res->qlock, NULL);

    pthread_attr_init(&attr);
    ret = pthread_create(
            &tid,&attr,atm_task_worker_func,res);
    if (ret != ATM_OK) {
        atm_log_rout(ATM_LOG_ERROR, 
                "task can not create worker thread");
        goto init_error;
    }
    res->tid = tid;
    return res;

init_error:
    if (wtasks != NULL) {
        atm_free(wtasks);
    }
    if (res != NULL) {
        pthread_cond_destroy(&res->qready);
        pthread_mutex_destroy(&res->qlock);
        atm_free(res);
    }
    return NULL;
}


static void 
atm_task_worker_free(void *worker)
{
    atm_task_worker_t *w = worker;
    if (w->tid != 0) {
        w->active = ATM_FALSE;
        pthread_join(w->tid, NULL);
    }
    atm_list_free(w->wtasks);
    pthread_cond_destroy(&w->qready);
    pthread_mutex_destroy(&w->qlock);
    atm_free(w);
}


static void 
atm_task_notify()
{
    char buf[1];

    buf[0] = 't';
    if (write(pipe_sent_fd,buf,1)!=1) {
        atm_log_rout(ATM_LOG_ERROR, 
                "task notify to pipe fail");
    }
}


static void 
atm_task_process_tasks(char *notify)
{
    atm_task_t *t = NULL;
    switch (notify[0]) {
        case 't':
            /* this is a single thread impl */
            while (ATM_TRUE) {
                t=atm_list_lpop(tasks);
                if (t == NULL) break;
                atm_task_worker_dispatch(t); 
            }
        break;
    }
}


static void 
atm_task_notify_recv(atm_event_t *ev)
{
    int processed = 0;
    int ret = 0;
    char buf[1];
    int fd = pipe_recv_fd;

    while (ATM_TRUE) {
        ret = read(fd, buf, 1);
        if (ret > 0) {
            if (!processed) {
                atm_task_process_tasks(buf);             
                processed++;
            }
            /* flush all buf */
            continue;
        } else {
            break;
        }
    }
}


static void
atm_task_nonblock_fd(int fd)
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
atm_task_pipe_init()
{
    int fds[2];
    atm_event_t *pe = NULL;
    int events = ATM_EVENT_NONE;

    if (pipe(fds)) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not create notify pipe");
        exit(1);
    }

    pipe_recv_fd = fds[0];
    atm_task_nonblock_fd(pipe_recv_fd);
    pipe_sent_fd = fds[1];
    atm_task_nonblock_fd(pipe_sent_fd);

    pe = atm_event_new(
            NULL,
            pipe_recv_fd,
            atm_task_notify_recv,
            NULL, 
            NULL); 

    events = EPOLLIN|EPOLLHUP|EPOLLET;
    atm_log("task initial events mask %u", events);
    atm_event_add_event(pe, events);
}


/*
 * Public
 * */
void 
atm_task_init()
{
    int nworker = 1;
    /* Yes we actrally need a thread pool */
    tasks = atm_list_new(
            ATM_TASK_T, 
            ATM_FREE_DEEP);
    atm_task_worker_init(nworker);
    /* Pipe the task event and reg to epoll*/
    atm_task_pipe_init();
}


atm_task_t * 
atm_task_new(void *load, 
        atm_int_t (*run)(atm_task_t *self))
{
    atm_task_t *t = NULL;
    t = atm_alloc(sizeof(atm_task_t));
    t->load = load;
    t->run = run;
    t->retry = ATM_FALSE;
    return t;
}


void 
atm_task_free(void *task)
{
    atm_task_t *t = task;
    if (t != NULL) {
        atm_free(t);
    }
}


void 
atm_task_dispatch(atm_task_t *task)
{
    atm_task_t *t = task;
    if (t != NULL) {
        atm_list_push(tasks, t);
        atm_task_notify();
    }
}
