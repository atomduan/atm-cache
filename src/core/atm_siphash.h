#ifndef _ATM_SIPHASH_H_INCLUDED_
#define _ATM_SIPHASH_H_INCLUDED_

#include <atm_core.h>


void
atm_siphash_init();

uint64_t
atm_siphash(char *input, atm_uint_t inlen);

uint64_t
atm_siphash_nocase(char *input, atm_uint_t inlen);


#endif /* _ATM_SIPHASH_H_INCLUDED_ */
