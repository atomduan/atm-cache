#include <atm_core.h>



static atm_list_t *tasks;


/*
 * Public
 * */


atm_T_t ATM_TASK_T = {
    ATM_T_INDEX_TASK,
    atm_task_spec,
    atm_task_match,
    atm_task_hash,
    atm_task_cmp,
    atm_task_str,
    atm_task_free,
};


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
void 
atm_task_init()
{
    /* Yes we actrally need a thread pool */
    tasks = atm_list_new(
            &ATM_TASK_T, 
            ATM_FREE_DEEP);
}


atm_task_t * 
atm_task_new(
        void *load, 
        atm_int_t (*run)(atm_task_t *self))
{
    atm_task_t *t = NULL;
    t = atm_alloc(sizeof(atm_task_t));
    t->load = load;
    t->run = run;
    return t;
}


void *
atm_task_spec(void *task)
{
    void *res = NULL;
    return res;
}


atm_bool_t 
atm_task_match(void *task1, void *task2)
{
    atm_bool_t res = ATM_FALSE;
    if (task1 == task2) {
        res = ATM_TRUE;
    }
    return res;
} 


uint64_t 
atm_task_hash(void *task)
{
    uint64_t res = 0;
    atm_task_t *t = NULL; 
    atm_str_t *t_str = NULL;

    t = (atm_task_t *) task;
    t_str = atm_str_ptr_str(t); 
    res = atm_hash(t_str->val,t_str->len);

    atm_str_free(t_str);
    return res;
}


atm_int_t 
atm_task_cmp(void *task1, void *task2)
{
    return ATM_CMP_EQ;
}


atm_str_t *
atm_task_str(void *task)
{
    atm_str_t *res = NULL;
    return res;
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
    }
    
    while (ATM_TRUE) {
        t=atm_list_lpop(tasks);
        if (t == NULL) break;
        t->run(t);
        atm_task_free(t);
    }
}
