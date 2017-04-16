#ifndef _ATM_CORE_H_INCLUDED_
#define _ATM_CORE_H_INCLUDED_

#include <atm_config.h>
#include <atm_string.h>
#include <atm_hash.h>
#include <atm_list.h>
#include <atmcache.h>

#ifdef ATM_UNIT_TEST
#include <atm_unit_test.h>
#endif

void *atm_malloc(size_t size);
void *atm_calloc(size_t nmemb, size_t size);
void atm_free(void *ptr);

#endif
