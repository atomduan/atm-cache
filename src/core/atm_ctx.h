#ifndef _ATM_CTX_H_INCLUDED_
#define _ATM_CTX_H_INCLUDED_

#include <atm_core.h>

extern atm_ctx_t *atm_ctx;

struct atm_ctx_s {
    pid_t           pid;
    atm_config_t   *config;
    atm_dict_t     *cache_dict;
    atm_uint_t      initial_memory_usage;
    /*
     * the service level daemon service
     * call back should be register here
     * suck as atm_task_monitor
     * this array will be pooled
     * by the main event loop
     * TODO, need implemented
     */
    atm_arr_t      *bg_routine;
};

void
atm_ctx_init();

void
atm_ctx_callback_loop();
#endif /* _ATM_CTX_H_INCLUDED_ */
