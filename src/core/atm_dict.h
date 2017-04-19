#ifndef _ATM_DICT_H_INCLUDED_
#define _ATM_DICT_H_INCLUDED_

#include <atm_core.h>


#define ATM_DICT_INITIAL_BUCKET_SIZE 256


struct atm_dict_T_s {
    atm_bool_t  (* match)(void *k1, void *k2);
    void        (* free_key)(void *v);
    void        (* free_value)(void *v);
};


struct atm_dict_s {
    atm_bool_t          shallow_free;
    atm_list_t        **bucket;
    atm_uint_t          bucket_size;
    atm_uint_t          size;
    atm_dict_T_t       *type;
};


struct atm_dict_entry_s {
    atm_dict_t         *dict;
    atm_list_t         *list;
    void               *key;
    void               *value;
};


void atm_dict_init();


atm_dict_t *atm_dict_new(atm_dict_T_t *type);
void atm_dict_free(void *d);


atm_bool_t atm_dict_contains(atm_dict_t *dict, void *key);
void *atm_dict_get(atm_dict_t *dict, void *key);
void atm_dict_set(atm_dict_t *dict, void *key, void *value);
void atm_dict_remove(atm_dict_t *dict, void *key);


#endif /* _ATM_DICT_H_INCLUDED_ */
