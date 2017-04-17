#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_


#include <atm_auto_headers.h>


#if (ATM_LINUX)
#include <atm_linux_config.h>
#endif


typedef size_t          atm_size_t;
typedef intptr_t        atm_int_t;
typedef uintptr_t       atm_uint_t;
typedef long            atm_long_t;
typedef long long       atm_llong_t;
typedef unsigned long   atm_ulong_t;

typedef char            atm_char_t;
typedef atm_char_t *    atm_str_t;
typedef struct {
    atm_size_t  len;
    atm_str_t   str;
} atm_string_t;

typedef uint8_t         atm_byte_t;


#define ATM_TRUE    1
#define ATM_FALSE   0

#endif
