#ifndef _ATM_SIPHASH_H_INCLUDED_
#define _ATM_SIPHASH_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>


void atm_siphash_init();
 

uint64_t atm_siphash(atm_str_t input, size_t inlen);
uint64_t atm_siphash_nocase(atm_str_t input, size_t inlen);


#endif
