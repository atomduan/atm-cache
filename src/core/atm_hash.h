#ifndef _ATM_HASH_H_INCLUDED_
#define _ATM_HASH_H_INCLUDED_

#include <atm_core.h>


void 
atm_hash_init();

uint64_t 
atm_hash(char *input, atm_uint_t inlen);

uint64_t 
atm_hash_nocase(char *input, atm_uint_t inlen);


#endif /* _ATM_HASH_H_INCLUDED_ */
