#ifndef _ATM_DICT_H_INCLUDED_
#define _ATM_DICT_H_INCLUDED_

#include <atm_core.h>


#define ATM_DICT_INITIAL_BUCKET_SIZE 256


/* this is a inner type 
 * so we do not define it in atm_types.h 
 * plz do not use it in other file, consciously
 * the global type should be define is atm_types.h Specific Tyes sect
 * */
typedef struct atm_dict_entry_s atm_dict_entry_t;


struct atm_dict_s {
    atm_bool_t          deep_free;
    atm_list_t        **bucket;
    atm_uint_t          bucket_size;
    atm_uint_t          size;
    atm_T_t            *k_type;
    atm_T_t            *v_type;
};


struct atm_dict_entry_s {
    atm_dict_t         *dict;
    atm_list_t         *list;
    void               *key;
    void               *val;
};


/* public dict type lifecycle */
void 
atm_dict_init();

atm_dict_t *
atm_dict_new(atm_T_t *k_type, atm_T_t *v_type);

void *
atm_dict_spec(void *dict);

atm_bool_t 
atm_dict_match(void *dict1, void *dict2); 

uint64_t 
atm_dict_hash(void *dict);

atm_int_t 
atm_dict_cmp(void *dict1, void *dict2);

atm_str_t *
atm_dict_str(void *dict);

void 
atm_dict_free(void *dict);


/* public funcs */
atm_bool_t 
atm_dict_contains(atm_dict_t *dict, void *key);

void *
atm_dict_get(atm_dict_t *dict, void *key);

void 
atm_dict_set(atm_dict_t *dict, void *key, void *val);

void 
atm_dict_del(atm_dict_t *dict, void *key);


#endif /* _ATM_DICT_H_INCLUDED_ */
