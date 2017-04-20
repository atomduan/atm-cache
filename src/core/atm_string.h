#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_core.h>


#define ATM_STR_TMN \0


/* 
 * Encapsutalted data type define:
 *      currently we only have string type
 */
struct atm_string_s {
    atm_str_t   str;
    atm_int_t   len;
};


/* public string type lifecycle */
void atm_string_init();
atm_string_t *atm_string_new(atm_str_t str);
void *atm_string_spec(void *str);
atm_bool_t atm_string_match(void *str1, void *str2); 
uint64_t atm_string_hash(void *str);
atm_int_t atm_string_compare(void *str1, void *str2);
atm_string_t *atm_string_string(void *str);
void atm_string_free(void *str);


/* public funcs */
atm_string_t *atm_string_fmt(atm_str_t str,...);
atm_string_t *atm_string_ptr_tostr();
atm_bool_t atm_string_equal(
        const atm_string_t *s1, const atm_string_t *s2);


#endif /* _ATM_STRING_H_INCLUDED_ */
