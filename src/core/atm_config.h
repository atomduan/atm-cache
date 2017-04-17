#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_


#include <atm_auto_headers.h>


#if (ATM_LINUX)
#include <atm_linux_config.h>
#endif


typedef intptr_t        atm_int_t;
typedef uintptr_t       atm_uint_t;
typedef long            atm_long_t;
typedef long long       atm_llong_t;
typedef unsigned long   atm_ulong_t;

typedef uint8_t         atm_byte_t;

typedef char *          atm_str_t;
typedef struct {
    size_t      len;
    atm_str_t   data;
} atm_string_t;

#endif
