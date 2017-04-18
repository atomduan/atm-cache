#ifndef _ATM_CONFIG_H_INCLUDED_
#define _ATM_CONFIG_H_INCLUDED_


#include <atm_auto_headers.h>


#if (ATM_LINUX)
#include <atm_linux_config.h>
#endif


/* 
 * Primitive data type define
 */
typedef int8_t          atm_bool_t;
#define ATM_TRUE    1
#define ATM_FALSE   0

/* int64_t or int32_t */
typedef int32_t         atm_int_t; 
/* In 64 system same to: size_t and unsigned long int
 * In 32 system: same to: size_t and unsigned int 
 */
typedef uint32_t        atm_uint_t;
typedef int64_t         atm_long_t;
typedef uint64_t        atm_ulong_t;

/*
 * Char and string relevant
 */
typedef char            atm_char_t;
typedef atm_char_t *    atm_str_t;

/* 
 * Byte oriented data type define:
 *      we recommand to use unint*_t 
 *      to represent bit wise data type 
 */
typedef uint8_t         atm_byte_t;/* same to u_char */


#endif
