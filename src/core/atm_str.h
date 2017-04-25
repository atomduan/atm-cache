#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_core.h>


#define ATM_STR_TMN \0
#define ATM_STR_FMT_MAX_LEN 1024

extern atm_T_t ATM_STR_T;


struct atm_str_s {
    char       *val;
    atm_int_t   len;
};


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
atm_str_string(void *str);

void 
atm_str_free(void *str);


/* public funcs */

atm_str_t *
atm_str_wrp(char *str, atm_uint_t len);

atm_bool_t 
atm_str_eq(atm_str_t *s1, atm_str_t *s2);

atm_str_t *
atm_str_fmt(char *fmt,...);

atm_str_t *
atm_str_vfmt(char *fmt, va_list args);

char *
atm_str_mtrim(char *s);

atm_str_t *
atm_str_ptr_str();


#endif /* _ATM_STRING_H_INCLUDED_ */
