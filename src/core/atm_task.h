#ifndef _ATM_TASK_H_INCLUDED_
#define _ATM_TASK_H_INCLUDED_

#include <atm_core.h>

#define ATM_TASK_WORK_ACTIVE    0
#define ATM_TASK_WORK_PASSIVE   1
#define ATM_TASK_WORK_RETIRED   2

#define ATM_TASK_HIGH_LOAD_THRESHOLD    10
#define ATM_TASK_LOW_LOAD_THRESHOLD    3

struct atm_task_s {
    void               *load;
    atm_int_t         (*run)(atm_task_t *self);
};

typedef struct {
    /* blocking queue */
    atm_queue_t        *blking_tasks;
    pthread_t           tid;
    atm_int_t           status;
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

void
atm_task_moniter_routine();


#endif /* _ATM_TASK_H_INCLUDED_ */
