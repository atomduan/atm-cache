#include <atm_core.h>
/*
 * Private
 * */
static void
atm_task_notify_handle(void *task); 

static void *
atm_task_worker_func(void *arg);

static void
atm_task_worker_init(int nworker);

static atm_task_worker_t *
atm_task_worker_new();

static void 
atm_task_worker_free(void *worker);


/* pipe define */
static atm_pipe_t *notify_pipe;

/* task type define for container */
static atm_T_t ATM_TASK_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_task_free,
};
static atm_T_t *ATM_TASK_T = &ATM_TASK_TYPE;


/* worker define for container*/
static atm_T_t ATM_TASK_WORKER_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_task_worker_free,
};
static atm_T_t *ATM_TASK_WORKER_T = &ATM_TASK_WORKER_TYPE;


/* worker list define */
static atm_list_t *workers;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_task_notify_handle(void *task)
{
    atm_task_t *t = task;
    atm_task_worker_t *curr_worker = NULL;

    curr_worker = atm_list_round(workers);
    if (curr_worker == NULL) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not get curr_worker");
        exit(1);
    }
    /* push t to worker's blocking queue */
    atm_queue_push(curr_worker->blking_tasks, t);
}


static void *
atm_task_worker_func(void *arg)
{
    void *res = NULL;
    atm_task_worker_t *worker = NULL;
    atm_queue_t *blking_tasks = NULL;
    atm_task_t *t = NULL;

    worker = arg;
    blking_tasks = worker->blking_tasks;
    while (worker->active) {
        /* blocking pop hasppens here */
        t = atm_queue_pop(worker->blking_tasks);
        if (t != NULL) {
            t->run(t);
            atm_task_free(t);
        }
    }
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
    atm_queue_t *blking_tasks = NULL;

    int ret;
    pthread_t tid;
    pthread_attr_t attr;

    res = atm_alloc(sizeof(atm_task_worker_t));
    res->blking_tasks = atm_queue_new(
                ATM_TASK_T,
                ATM_FREE_DEEP,
                ATM_QUEUE_BLOCK);
    res->active = ATM_TRUE;
    res->tid = 0;

    pthread_attr_init(&attr);
    ret = pthread_create(&tid,&attr,
            atm_task_worker_func,res);

    if (ret != ATM_OK) {
        atm_log_rout(ATM_LOG_ERROR, 
                "task create worker fail");
        goto init_exception;
    }
    res->tid = tid;
    return res;

init_exception:
    if (blking_tasks != NULL) {
        atm_queue_free(blking_tasks);
    }
    if (res != NULL) {
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
    atm_queue_free(w->blking_tasks);
    atm_free(w);
}


/*
 * Public
 * */
void 
atm_task_init()
{
    int nworker = 1;
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
