#ifndef _ATM_CTX_H_INCLUDED_
#define _ATM_CTX_H_INCLUDED_

#include <atm_core.h>

extern atm_ctx_t *atm_ctx;

struct atm_ctx_s {
    pid_t           pid;
    atm_config_t   *config;
    atm_dict_t     *cache_dict;
    atm_uint_t      initial_memory_usage;
};

void
atm_ctx_init();

#endif /* _ATM_CTX_H_INCLUDED_ */
