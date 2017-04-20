#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_

#include <atm_auto_headers.h>

#if (ATM_LINUX)
#include <atm_linux_config.h>
#endif

#define ATM_OK          0
#define ATM_ERROR      -1
#define ATM_FATAL      -2

#define ATM_TRUE        1
#define ATM_FALSE       0

#define ATM_COMP_LESS  -1
#define ATM_COMP_EQUAL  0
#define ATM_COMP_MORE   1


#endif /* _ATM_CONFIG_H_INCLUDED_ */
