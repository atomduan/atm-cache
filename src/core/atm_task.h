#ifndef _ATM_TASK_H_INCLUDED_
#define _ATM_TASK_H_INCLUDED_

#include <atm_core.h>

#define ATM_TASK_WORKER_BLOCKING_INTVAL 1


struct atm_task_s {
    atm_bool_t          active;
    void               *load;
    atm_int_t         (*run)(atm_task_t *self);
};

typedef struct {
    /* blocking queue */
    atm_queue_t        *blking_tasks;
    atm_bool_t          active;
    pthread_t           tid;
} atm_task_worker_t;

void 
atm_task_init();

atm_task_t * 
atm_task_new(void *load, 
        atm_int_t (*run)(atm_task_t *self));

void 
atm_task_free(void *task);

void 
atm_task_dispatch(atm_task_t *task);


#endif /* _ATM_TASK_H_INCLUDED_ */
