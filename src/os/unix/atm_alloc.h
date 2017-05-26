#ifndef _ATM_ALLOC_H_INCLUDED_
#define _ATM_ALLOC_H_INCLUDED_

#include <atm_core.h>


#define ATM_MEM_ZERO    0


void *
atm_alloc(atm_uint_t size);

void *
atm_calloc(atm_uint_t nmemb, atm_uint_t size);

void *
atm_realloc(void *p, atm_uint_t osz, atm_uint_t nsz);

void
atm_free(void *ptr);

atm_uint_t
atm_phy_mems();


#endif /* _ATM_ALLOC_H_INCLUDED_ */
