#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_core.h>


#define ATM_STR_TMN \0

typedef struct atm_string_s atm_string_t;
/* 
 * Encapsutalted data type define:
 *      currently we only have string type
 */
struct atm_string_s {
    atm_str_t   str;
    atm_int_t   len;
};


void atm_string_init();

atm_string_t *atm_string_new(atm_str_t str);
void atm_string_free(atm_string_t *e);

atm_bool_t atm_string_equal(
        const atm_string_t *s1, const atm_string_t *s2);


#endif /* _ATM_STRING_H_INCLUDED_ */
