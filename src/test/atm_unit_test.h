#ifndef _ATM_UNITEST_H_INCLUDED_
#define _ATM_UNITEST_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>

#include <atm_test_hash.h>
#include <atm_test_process.h>

/*
 * The entry point of unit test system
 */
int atm_unit_test_proc(int argc, char **argv);

void atm_test_report(atm_str_t argc, atm_uint_t result);

#endif
