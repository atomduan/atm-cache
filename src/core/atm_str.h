#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_core.h>


#define ATM_STR_TMN \0
#define ATM_STR_MAXLEN 512
#define ATM_WORDLEN 8


extern atm_T_t *ATM_STR_T;

typedef struct atm_str_hdr_s    atm_str_hdr;  
struct __attribute__ ((__packed__)) atm_str_hdr_s {
    atm_uint_t  cap;
    atm_uint_t  len;
};


/* public string type lifecycle */
atm_str_t
atm_str_new(char *str);

atm_bool_t 
atm_str_match(void *str1, void *str2); 

uint64_t 
atm_str_hash(void *str);

atm_int_t 
atm_str_cmp(void *str1, void *str2);

atm_str_t
atm_str_string(void *str);

void 
atm_str_free(void *str);


/* public funcs */
atm_uint_t
atm_str_len(atm_str_t str);

atm_bool_t 
atm_str_eq(atm_str_t str1, atm_str_t str2);

atm_bool_t 
atm_str_eqs(atm_str_t str1, char *s);

atm_str_t
atm_str_fmt(char *fmt,...);

char *
atm_str_mtrim(char *s);

atm_str_t
atm_str_ptr(void *p);

atm_str_t
atm_str_cat(atm_str_t dest, char *s);

atm_str_t *
atm_str_split(char *s, int len);


#endif /* _ATM_STRING_H_INCLUDED_ */
