#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>


/* 
 * Encapsutalted data type define:
 *      currently we only have string type
 */
typedef struct {
    atm_str_t   str;
    atm_int_t   len;
} atm_string_t;

#define ATM_STR_TMN \0

void atm_string_free(atm_string_t *s);
atm_bool_t atm_string_equal(
        const atm_string_t *s1, const atm_string_t *s2);

#endif
