#include <atm_core.h>
/*
 * Private
 * */
static void
atm_queue_nonempty_wait(atm_queue_t *queue);

/* resolution is millisecond(ms) */
static atm_int_t loop_timeout = 1000;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_queue_nonempty_wait(atm_queue_t *queue)
{
    atm_uint_t btyp;
    atm_queue_t *q = NULL;
    atm_int_t timeout = loop_timeout;

    struct timespec ts;
    struct timeval now;

    if (queue != NULL) {
        q = queue;
        btyp = q->blk_type;

        if (btyp == ATM_QUEUE_BLOCK) {
            gettimeofday(&now, NULL);
            ts.tv_sec = now.tv_sec + timeout;
            ts.tv_nsec = now.tv_usec * 1000;

            pthread_mutex_lock(&q->qlock);
            while (atm_queue_size(q) == 0) {
                atm_log("enter cond wait q addr is %p", q);
                atm_log("blking queue wait...");
                //pthread_cond_timedwait(&q->qready,&q->qlock,&ts);
                pthread_cond_wait(&q->qready,&q->qlock);
            }
            atm_log("blocking queue awake, q addr is %p", q);
            pthread_mutex_unlock(&q->qlock);
        } else {
            atm_log_rout(ATM_LOG_FATAL, 
                "waiting for nonblock queue");
            exit(1);
        }
    }
}


/*
 * Public
 * */
atm_queue_t *
atm_queue_new(atm_T_t *v_type, 
        atm_uint_t f_type, atm_uint_t blk_type)
{
    atm_queue_t *res = NULL;
    res = atm_alloc(sizeof(atm_queue_t));
    res->blk_type = blk_type;
    res->vals = atm_list_new(v_type, f_type);
    switch (res->blk_type) {
        case ATM_QUEUE_BLOCK:
            atm_log("queue new, init qready res addr is %p", res);
            pthread_cond_init(&res->qready, NULL);
            pthread_mutex_init(&res->qlock, NULL);
        break;
        case ATM_QUEUE_NONBLOCK:
            /* NOTHING TO DO*/
        break;
        default:
            goto type_exception;
        break;
    }
    return res;

type_exception:
    atm_queue_free(res);
    res = NULL;
    return res;
}


void
atm_queue_free(void *queue)
{
    atm_queue_t *q = queue;
    atm_list_free(q->vals);
    if (q->blk_type == ATM_QUEUE_BLOCK) {
        pthread_cond_destroy(&q->qready);
        pthread_mutex_destroy(&q->qlock);
    }
    atm_free(q);
}


void
atm_queue_push(atm_queue_t *queue, void *val)
{
    atm_uint_t btyp;
    atm_queue_t *q = NULL;

    if (queue != NULL) {
        q = queue;
        btyp = q->blk_type;
        if (btyp == ATM_QUEUE_NONBLOCK) {
            atm_list_push(q->vals, val);
        } else
        if (btyp == ATM_QUEUE_BLOCK) {
            pthread_mutex_lock(&q->qlock);
            atm_list_push(q->vals, val);
            pthread_mutex_unlock(&q->qlock);
            atm_log("qready push, signal queue, addr is %p", q);
            pthread_cond_signal(&q->qready);
        }
    }
}


void *
atm_queue_peek(atm_queue_t *queue)
{
    void *res = NULL;
    atm_uint_t btyp;
    atm_queue_t *q = NULL;
    if (queue != NULL) {
        q = queue;
        btyp = q->blk_type;
        if (btyp == ATM_QUEUE_NONBLOCK) {
            res = atm_list_lpeek(q->vals);
        } else
        if (btyp == ATM_QUEUE_BLOCK) {
            atm_queue_nonempty_wait(q);
            res = atm_list_lpeek(q->vals);
        }
    }
    return res;
}


void *
atm_queue_pop(atm_queue_t *queue)
{
    void *res = NULL;
    atm_uint_t btyp;
    atm_queue_t *q = NULL;
    if (queue != NULL) {
        q = queue;
        btyp = q->blk_type;
        if (btyp == ATM_QUEUE_NONBLOCK) {
            res = atm_list_lpop(q->vals);
        } else
        if (btyp == ATM_QUEUE_BLOCK) {
            atm_queue_nonempty_wait(q);
            res = atm_list_lpop(q->vals);
        }
    }
    return res;
}

atm_uint_t
atm_queue_size(atm_queue_t *queue)
{
    atm_uint_t res = 0;
    if (queue != NULL) {
        res = queue->vals->size;
    }
    return res;
}
