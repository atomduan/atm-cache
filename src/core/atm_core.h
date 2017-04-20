#ifndef _ATM_CORE_H_INCLUDED_
#define _ATM_CORE_H_INCLUDED_

#include <atm_config.h>
#include <atm_types.h>


#include <atm_string.h>
#include <atm_list.h>
#include <atm_dict.h>
#include <atm_siphash.h>
#include <atm_logger.h>
#include <atmcache.h>


#ifdef ATM_UNIT_TEST
#include <atm_unit_test.h>
#endif


void *
atm_malloc(atm_uint_t size);

void *
atm_calloc(atm_uint_t nmemb, atm_uint_t size);

void 
atm_free(void *ptr);


#endif /* _ATM_CORE_H_INCLUDED_ */
