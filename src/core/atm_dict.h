#ifndef _ATM_DICT_H_INCLUDED_
#define _ATM_DICT_H_INCLUDED_

#include <atm_core.h>


#define ATM_DICT_INITIAL_BUCKET_SIZE 256


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
    void               *value;
};


void atm_dict_init();


atm_dict_t *atm_dict_new(atm_T_t *k_type, atm_T_t *v_type);
void atm_dict_free(void *dict);


atm_bool_t atm_dict_contains(atm_dict_t *dict, void *key);
void *atm_dict_get(atm_dict_t *dict, void *key);
void atm_dict_set(atm_dict_t *dict, void *key, void *value);
void atm_dict_del(atm_dict_t *dict, void *key);


#endif /* _ATM_DICT_H_INCLUDED_ */
