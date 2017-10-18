#include <atm_core.h>
/*
 * Private
 * */
static atm_uint_t
atm_task_worker_load(atm_task_worker_t *w);
static atm_bool_t
atm_task_del_worker();
static atm_bool_t
atm_task_add_worker();
static atm_uint_t
atm_task_load();
static atm_task_worker_t *
atm_task_get_active_worker();
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
static pthread_mutex_t worker_lock;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_uint_t
atm_task_worker_load(atm_task_worker_t *worker)
{
    atm_uint_t res;
    atm_queue_t *q;
    q = worker->blking_tasks;
    res = atm_queue_size(q);
    return res;
}


static atm_bool_t
atm_task_del_worker()
{
    atm_bool_t res = ATM_FALSE;
    atm_uint_t i = 0;
    atm_task_worker_t *worker;
    atm_config_t *conf = atm_ctx->config;
    /*
     * TODO a urgly implementation
     */
    atm_uint_t nworker = (atm_uint_t)conf->workernum;

    if (workers->length <= nworker)
        return res;

    pthread_mutex_lock(&worker_lock);
    for (i=0; i<workers->length; ++i) {
        worker = (atm_task_worker_t *)atm_arr_get(workers, i);
        if (worker != NULL) {
            if (worker->status == ATM_TASK_WORK_ACTIVE) {
                worker->status = ATM_TASK_WORK_PASSIVE;
                res = ATM_TRUE;
                break;
            }
        }
    }
    pthread_mutex_unlock(&worker_lock);
    return res;
}


static atm_bool_t
atm_task_add_worker()
{
    atm_bool_t res = ATM_FALSE;
    int ret;
    pthread_t tid;
    pthread_attr_t attr;
    atm_task_worker_t *w;

    pthread_mutex_lock(&worker_lock);
    w = atm_task_worker_new();
    if (w != NULL) {
        w->status = ATM_TASK_WORK_ACTIVE;
        pthread_attr_init(&attr);
        ret = pthread_create(&tid,&attr,
            atm_task_worker_func,w);
        if (ret == ATM_OK) {
            atm_arr_add(workers, &w);
            res = ATM_TRUE;
        } else {
            w->status = ATM_TASK_WORK_RETIRED;
            atm_log_rout(ATM_LOG_ERROR,
                "task create worker fail");
            atm_task_worker_free(w);
        }
    }
    pthread_mutex_unlock(&worker_lock);
    return res;
}


static atm_uint_t
atm_task_load()
{
    atm_uint_t i = 0;
    atm_task_worker_t *worker;
    atm_uint_t ts = 0;
    atm_uint_t ws = 0;

    pthread_mutex_lock(&worker_lock);
    for (i=0; i<workers->length; ++i) {
        worker = (atm_task_worker_t *)atm_arr_get(workers, i);
        if (worker != NULL) {
            if (worker->status == ATM_TASK_WORK_ACTIVE) {
                ws++;
                ts += atm_task_worker_load(worker);
            }
        }
    }
    pthread_mutex_unlock(&worker_lock);
    return ts/ws;
}


static atm_task_worker_t *
atm_task_get_active_worker()
{
    atm_task_worker_t *res = NULL;

    atm_task_worker_t **wks;
    atm_task_worker_t *worker;
    atm_task_worker_t *wtmp;
    atm_arr_t *workers_tmp;
    atm_uint_t worker_nums = 0;
    atm_uint_t wi = 0;
    atm_uint_t i = 0;

    worker_nums = workers->length;
    if (worker_nums > 0) {
        wi = ++workers_round % worker_nums;
    }
    wks = atm_arr_get(workers, wi);
    worker = *wks;

    if (worker->status == ATM_TASK_WORK_RETIRED) {
        /*
         * theoretically, there should be only one
         * exec path which is same to epoll
         */
        pthread_mutex_lock(&worker_lock);
        /*triger retired worker recycling*/
        workers_tmp = atm_arr_new(sizeof(atm_task_worker_t *));
        for (i=0; i<workers->length; ++i) {
            wtmp = atm_arr_get(workers, i);
            if (wtmp->status != ATM_TASK_WORK_RETIRED) {
                atm_arr_add(workers_tmp, wtmp);
            } else {
                atm_task_worker_free(wtmp);
            }
        }
        atm_arr_free(workers);
        workers = workers_tmp;
        pthread_mutex_unlock(&worker_lock);
    }

    if (worker == NULL) {
        atm_log_rout(ATM_LOG_FATAL,
                "can not get "
                "curr_worker wi[%lu] "
                "worker_nums[%lu]",
                wi,worker_nums);
        exit(ATM_ERROR);
    }

    res = worker;
    return res;
}


static void
atm_task_notify_handle(void *task)
{
    atm_task_t *t = task;
    atm_task_worker_t *curr_worker;

    atm_log("#####atm_task_notify_handle task recv %p", task);
    curr_worker = atm_task_get_active_worker();
    /* push t to worker's blocking queue */
    atm_log("#####notified worker queue is %p, task is %p",
            curr_worker->blking_tasks, task);
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
    while (worker->status != ATM_TASK_WORK_RETIRED) {
        /* blocking pop hasppens here */
        t = atm_queue_pop(worker->blking_tasks);
        if (t != NULL) {
            t->run(t);
            atm_task_free(t);
        } else {
            /* set worker to retired */
            if (worker->status == ATM_TASK_WORK_PASSIVE) {
                worker->status = ATM_TASK_WORK_RETIRED;
                break;
            }
        }
    }
    return res;
}


static void
atm_task_worker_init(int nworker)
{
    int i = 0;
    workers = atm_arr_new(sizeof(atm_task_worker_t *));
    for (i=0; i<nworker; ++i) {
        atm_task_add_worker();
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
    res->status = ATM_TASK_WORK_ACTIVE;
    res->tid = 0;
    return res;
}


static void
atm_task_worker_free(void *worker)
{
    pthread_t curr_thread = pthread_self();
    atm_task_worker_t *w = worker;
    if (w->tid != 0) {
        w->status = ATM_TASK_WORK_RETIRED;
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
    atm_config_t *conf = atm_ctx->config;
    int nworker = conf->workernum;
    /* workers init */
    atm_task_worker_init(nworker);
    /* dispatch pipe init*/
    notify_pipe = atm_pipe_new();
    /* trust pipe event manage to epoll */
    atm_pipe_event_init(notify_pipe);
    pthread_mutex_init(&worker_lock, NULL);
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


/*
 * should be invoked periodically
 * should be garenteed thread-safe and
 * run excludely from atm_task_get_active_worker func
 * should be the type of atm_ctx_callback
 */
void
atm_task_moniter_routine()
{
    atm_int_t load = atm_task_load();
    if (load > ATM_TASK_HIGH_LOAD_THRESHOLD) {
        atm_task_add_worker();
    } else
    if (load < ATM_TASK_LOW_LOAD_THRESHOLD) {
        atm_task_del_worker();
    }
}
