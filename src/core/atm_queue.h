#ifndef _ATM_QUEUE_H_INCLUDED_
#define _ATM_QUEUE_H_INCLUDED_

#include <atm_core.h>

#define ATM_QUEUE_BLOCK         1
#define ATM_QUEUE_NONBLOCK      2

/* 
 * blocking queue struct
 * */
struct atm_queue_s {
    atm_uint_t          blk_type;
    atm_list_t         *vals;
    pthread_cond_t      qready;
    pthread_mutex_t     qlock;
};


atm_queue_t *
atm_queue_new(atm_T_t *v_type, 
        atm_uint_t f_type, atm_uint_t blk_type);

void
atm_queue_free(void *queue);

void
atm_queue_push(atm_queue_t *queue, void *val);

void *
atm_queue_peek(atm_queue_t *queue);

void *
atm_queue_pop(atm_queue_t *queue);

atm_uint_t
atm_queue_size(atm_queue_t *queue);


#endif /* _ATM_QUEUE_H_INCLUDED_ */
