#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_

#include <atm_core.h>

void 
atm_config_init();

char *
atm_config_get(char *k);

void 
atm_config_set(char *k, char *v);

void 
atm_config_del(char *k);

#endif /* _ATM_CONFIG_H_INCLUDED_ */
