#include <atm_core.h>
/*
 * Private
 * */
static atm_list_t *tasks;


static atm_T_t ATM_TASK_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_task_free,
};

static atm_T_t *ATM_TASK_T = &ATM_TASK_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
void 
atm_task_init()
{
    /* Yes we actrally need a thread pool */
    tasks = atm_list_new(
            ATM_TASK_T, 
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
