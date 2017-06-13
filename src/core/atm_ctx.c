#include <atm_core.h>
/*
 * Public
 * */
atm_ctx_t *atm_ctx;


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
}
