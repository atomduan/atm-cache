#include <atm_core.h>
/*
 * Public
 * */
atm_ctx_t *atm_ctx;
atm_uint_t loop_count;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
void
atm_ctx_init()
{
    atm_ctx = atm_alloc(sizeof(atm_ctx_t));
    atm_ctx->config = atm_config;
    atm_ctx->pid = getpid();
    atm_ctx->cache_dict = atm_dict_new(
            ATM_OBJ_T,
            ATM_OBJ_T,
            ATM_FREE_DEEP);
    atm_ctx->bg_routine = atm_arr_new(sizeof(atm_ctx_callback));
    /*
     * register system level call back functions
     */
    atm_arr_add(atm_ctx->bg_routine,
            atm_task_moniter_routine);
    loop_count = 0;
}

void
atm_ctx_callback_loop()
{
    atm_uint_t i = 0;
    atm_arr_t *cbks;
    atm_ctx_callback func;
    if (atm_ctx!=NULL && ++loop_count%10000==0) {
        cbks = atm_ctx->bg_routine;
        if (cbks != NULL) {
            for (i=0; i<cbks->length; ++i) {
                func = atm_arr_get(cbks, i);
                if (func != NULL) (*func)();
            }
        }
    }
}
