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


/* worker list define */
static atm_arr_t *workers;
static atm_uint_t workers_round;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_task_notify_handle(void *task)
{
    atm_uint_t worker_nums = 0;
    atm_uint_t wi = 0;
    atm_task_t *t = task;
    atm_task_worker_t **wks;
    atm_task_worker_t *curr_worker;

    worker_nums = workers->length;
    if (worker_nums > 0) {
        wi = ++workers_round % worker_nums; 
    }

    wks = atm_arr_get(workers, wi);
    curr_worker = *wks;
    if (curr_worker == NULL) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not get curr_worker");
        exit(1);
    }
    /* push t to worker's blocking queue */
    atm_log("notified worker queue is %p", 
            curr_worker->blking_tasks);
    atm_queue_push(curr_worker->blking_tasks, t);
}


static void *
atm_task_worker_func(void *arg)
{
    void *res = NULL;
    atm_task_worker_t *worker;
    atm_task_t *t;

    worker = arg;
    worker->tid = pthread_self();
    atm_log("worker_fuc enter, worker tid[%u]", worker->tid);
    while (worker->active) {
        /* blocking pop hasppens here */
        t = atm_queue_pop(worker->blking_tasks);
        if (t != NULL) {
            t->run(t);
            atm_task_free(t);
        }
    }
    atm_task_worker_free(worker);
    return res;
}


static void
atm_task_worker_init(int nworker)
{
    int ret;
    pthread_t tid;
    pthread_attr_t attr;
    atm_task_worker_t *w;

    workers = atm_arr_new(sizeof(atm_task_worker_t *));
    for (int i=0; i<nworker; ++i) {
        w = atm_task_worker_new();
        if (w != NULL) {
            w->active = ATM_TRUE;
            pthread_attr_init(&attr);
            ret = pthread_create(&tid,&attr,
                atm_task_worker_func,w);

            if (ret == ATM_OK) {
                atm_arr_add(workers, &w);
                continue;
            } else {
                w->active = ATM_FALSE;
                atm_log_rout(ATM_LOG_ERROR, 
                    "task create worker fail");
                atm_task_worker_free(w);
                continue;
            }
        }
    }
}


static atm_task_worker_t *
atm_task_worker_new()
{
    atm_task_worker_t *res = NULL;

    res = atm_alloc(sizeof(atm_task_worker_t));
    res->blking_tasks = atm_queue_new(
                ATM_TASK_T,
                ATM_FREE_DEEP,
                ATM_QUEUE_BLOCK);
    res->active = ATM_FALSE;
    res->tid = 0;
    return res;
}


static void 
atm_task_worker_free(void *worker)
{
    pthread_t curr_thread = pthread_self();
    atm_task_worker_t *w = worker;
    if (w->tid != 0) {
        w->active = ATM_FALSE;
        /*
         * if the worker not kill him self
         * it should wait for working thread to finish
         */
        if (!pthread_equal(curr_thread,w->tid)) {
            atm_log("worker is going to free by diff threads[%lu],"
                    "working thread is [%lu]",curr_thread, w->tid);
            pthread_join(w->tid, NULL);
        }
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
