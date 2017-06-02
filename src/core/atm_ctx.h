#ifndef _ATM_CTX_H_INCLUDED_
#define _ATM_CTX_H_INCLUDED_

#include <atm_core.h>

extern atm_ctx_t *atm_ctx;

struct atm_ctx_s {
    /* General */
    atm_config_t   *config;
    /* Cache dict */
    atm_dict_t     *cache_dict;
};

#endif /* _ATM_CTX_H_INCLUDED_ */
