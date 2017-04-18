#ifndef _ATM_UNITEST_H_INCLUDED_
#define _ATM_UNITEST_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>

#include <atm_test_dict.h>
#include <atm_test_siphash.h>
#include <atm_test_process.h>

#define ATM_TEST_FAIL 0
#define ATM_TEST_PASS 1

/*
 * The entry point of unit test system
 */
int atm_unit_test_proc(int argc, char **argv);

void atm_test_report(atm_str_t argc, atm_uint_t result);

#endif
