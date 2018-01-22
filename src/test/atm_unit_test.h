#ifndef _ATM_UNITEST_H_INCLUDED_
#define _ATM_UNITEST_H_INCLUDED_

#include <atm_core.h>


#define ATM_TEST_FAIL 0
#define ATM_TEST_PASS 1


#define ATM_TEST_SIPHASH_ON     0
#include <atm_test_siphash.h>

#define ATM_TEST_DICT_ON        0
#include <atm_test_dict.h>

#define ATM_TEST_EVENT_ON       0
#include <atm_test_event.h>

#define ATM_TEST_STR_ON         0
#include <atm_test_str.h>

#define ATM_TEST_TIME_ON        0
#include <atm_test_time.h>

#define ATM_TEST_ARR_ON         0
#include <atm_test_arr.h>

#define ATM_TEST_FILE_ON        0
#include <atm_test_file.h>

#if (ATM_CLIENT_MOD)
    #define ATM_TEST_CLIENT_ON      1
    #include <atm_test_client.h>
#else
    #define ATM_TEST_SERVER_ON      1
    #include <atm_test_server.h>
#endif


/*
 * The entry point of unit test system
 */
void
atm_test_suit(int argc, char **argv);

void
atm_test_report(char *argc, atm_uint_t res);


#endif
