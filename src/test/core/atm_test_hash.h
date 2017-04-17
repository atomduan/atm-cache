#ifndef _ATM_TEST_HASH_H_INCLUDED_
#define _ATM_TEST_HASH_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>

#define ATM_TEST_HASH_FAIL 0
#define ATM_TEST_HASH_PASS 1

atm_uint_t atm_test_hash_suit();
atm_uint_t atm_test_hash_init();
atm_uint_t atm_test_hash_contains();
atm_uint_t atm_test_hash_key_func();
atm_uint_t atm_test_hash_get();
atm_uint_t atm_test_hash_set();
atm_uint_t atm_test_hash_remove();
atm_uint_t atm_test_siphash();

#endif
