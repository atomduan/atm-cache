#ifndef _ATM_STRING_H_INCLUDED_
#define _ATM_STRING_H_INCLUDED_

#include <atm_core.h>

#define ATM_STR_MAXLEN 512
#define ATM_WORDLEN 8


extern atm_T_t *ATM_STR_T;

typedef struct atm_str_hdr_s    atm_str_hdr_t;
struct __attribute__ ((__packed__)) atm_str_hdr_s {
    atm_uint_t  cap;
    atm_uint_t  len;
    char        val[];
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
atm_str_t
atm_str_dup(atm_str_t str);

atm_uint_t
atm_str_len(atm_str_t str);

atm_bool_t
atm_str_eq(atm_str_t str1, atm_str_t str2);

atm_bool_t
atm_str_eqs(atm_str_t str1, char *s);

atm_str_t
atm_str_fmt(char *fmt,...);

atm_str_t
atm_str_trim(atm_str_t str, const char *cset);

atm_str_t
atm_str_ptr(void *p);

atm_str_t
atm_str_cats(atm_str_t dest, char *s);

atm_str_t
atm_str_cats_dup(atm_str_t dest, char *s);

atm_str_t
atm_str_cat(atm_str_t dest, atm_str_t src);

atm_str_t
atm_str_cat_dup(atm_str_t dest, atm_str_t src);

atm_str_t *
atm_str_split(char *s, char sep, int len);

atm_int_t
atm_str_split_size(atm_str_t *argv);

void
atm_str_split_free(atm_str_t *argv);

void
atm_str_setlen(atm_str_t str, atm_uint_t len);

atm_str_t
atm_str_tolower(atm_str_t str);

atm_str_t
atm_str_toupper(atm_str_t str);

atm_bool_t
atm_str_isempty(atm_str_t str);


#endif /* _ATM_STRING_H_INCLUDED_ */
