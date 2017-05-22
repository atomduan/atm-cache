#ifndef _ATM_ALLOC_H_INCLUDED_
#define _ATM_ALLOC_H_INCLUDED_

#include <atm_core.h>


#define ATM_MEM_ZERO    0


void *
atm_alloc(atm_uint_t size);

void *
atm_calloc(atm_uint_t nmemb, atm_uint_t size);

void *
atm_realloc(void *p, atm_uint_t size);

void 
atm_free(void *ptr);


#endif /* _ATM_ALLOC_H_INCLUDED_ */
