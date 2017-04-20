#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_core.h>


#define ATM_STR_TMN \0


/* public string type lifecycle */
void 
atm_str_init();

atm_str_t *
atm_str_new(char *str);

void *
atm_str_spec(void *str);

atm_bool_t 
atm_str_match(void *str1, void *str2); 

uint64_t 
atm_str_hash(void *str);

atm_int_t 
atm_str_cmp(void *str1, void *str2);

atm_str_t *
atm_str_str(void *str);

void 
atm_str_free(void *str);


/* public funcs */
atm_str_t *
atm_str_fmt(char *str,...);

atm_str_t *
atm_str_ptr_tostr();

atm_bool_t 
atm_str_equal(const atm_str_t *s1, const atm_str_t *s2);


#endif /* _ATM_STRING_H_INCLUDED_ */
