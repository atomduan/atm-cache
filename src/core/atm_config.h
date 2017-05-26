#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_

#include <atm_core.h>

void
atm_config_init();

atm_str_t
atm_config_get(atm_str_t k);

void
atm_config_set(atm_str_t k, atm_str_t v);

void
atm_config_del(atm_str_t k);

#endif /* _ATM_CONFIG_H_INCLUDED_ */
