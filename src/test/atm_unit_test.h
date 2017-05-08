#ifndef _ATM_UNITEST_H_INCLUDED_
#define _ATM_UNITEST_H_INCLUDED_

#include <atm_core.h>


#define ATM_TEST_FAIL 0
#define ATM_TEST_PASS 1


#define ATM_TEST_SIPHASH_ON     0
#include <atm_test_siphash.h>

#define ATM_TEST_DICT_ON        1
#include <atm_test_dict.h>

#define ATM_TEST_EVENT_ON       0
#include <atm_test_event.h>

#define ATM_TEST_STR_ON         1
#include <atm_test_str.h>


/*
 * The entry point of unit test system
 */
void
atm_test_suit(int argc, char **argv); 

void 
atm_test_report(char *argc, atm_uint_t res);


#endif
