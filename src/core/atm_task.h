#ifndef _ATM_TASK_H_INCLUDED_
#define _ATM_TASK_H_INCLUDED_

#include <atm_core.h>


struct atm_task_s {
    void        *load;
    atm_int_t  (*run)(atm_task_t *self);
};


void 
atm_task_init();

atm_task_t * 
atm_task_new(
        void *load, 
        atm_int_t (*run)(atm_task_t *self));

void *
atm_task_spec(void *task);

atm_bool_t 
atm_task_match(void *task1, void *task2); 

uint64_t 
atm_task_hash(void *task);

atm_int_t 
atm_task_cmp(void *task1, void *task2);

atm_str_t *
atm_task_str(void *task);

void 
atm_task_free(void *task);

void 
atm_task_dispatch(atm_task_t *task);


#endif /* _ATM_TASK_H_INCLUDED_ */
