#include <atm_core.h>
/*
 * Private
 * */
static void
atm_task_notify_handle(void *task); 

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


/* pipe define */
static atm_pipe_t *notify_pipe;


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
static pthread_mutex_t workers_lock = PTHREAD_MUTEX_INITIALIZER;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_task_notify_handle(void *task)
{
    atm_task_t *t = task;
    atm_list_t  *wts = NULL;
    atm_task_worker_t *curr_worker = NULL;

    pthread_mutex_lock(&workers_lock);
    curr_worker = atm_list_round(workers);
    pthread_mutex_unlock(&workers_lock);

    if (curr_worker == NULL) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not get curr_worker");
        exit(1);
    }

    pthread_mutex_lock(&curr_worker->qlock);
    atm_log("dispatch one task to worker...");
    wts = curr_worker->wtasks;
    atm_list_push(wts, t);
    pthread_mutex_unlock(&curr_worker->qlock);
    atm_log("signal worker wake from blking...");
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
                atm_task_notify_handle(t);
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
    ts.tv_sec = now.tv_sec + timeout;
    ts.tv_nsec = now.tv_usec * 1000;

    wtasks = worker->wtasks;
    pthread_mutex_lock(&worker->qlock);
    while (wtasks->size == 0) {
        atm_log("worker blk waiting....");
        //pthread_cond_timedwait(&worker->qready,&worker->qlock,&ts);
        pthread_cond_wait(&worker->qready,&worker->qlock);
    }
    atm_log("worker get one task....");
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


/*
 * Public
 * */
void 
atm_task_init()
{
    int nworker = 10;
    /* workers init */
    atm_task_worker_init(nworker);

    /* dispatch pipe init*/
    notify_pipe = atm_pipe_new();
    /* trust pipe event manage to epoll */
    atm_pipe_event_init(notify_pipe);
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
    if (task != NULL) {
        atm_pipe_notify(
            notify_pipe,
            task,
            atm_task_notify_handle);
    }
}
