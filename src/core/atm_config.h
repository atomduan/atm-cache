#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <atm_auto_config.h>
#include <atm_auto_headers.h>

typedef intptr_t        atm_int_t;
typedef uintptr_t       atm_uint_t;
typedef long            atm_long_t;
typedef unsigned long   atm_ulong_t;

typedef struct {
    size_t len;
    u_char *data;
} atm_str_t;

#endif
