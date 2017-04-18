#ifndef _ATM_DICT_H_INCLUDED_
#define _ATM_DICT_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>


#define ATM_DICT_INITIAL_BUCKET_SIZE 256


typedef struct atm_dict_s           atm_dict_t;
typedef struct atm_dict_entry_s     atm_dict_entry_t;
typedef atm_dict_entry_t *          atm_dict_bucket_t;


struct atm_dict_s {
    atm_dict_bucket_t  *table;
    atm_uint_t          size;
};


struct atm_dict_entry_s {
    atm_string_t       *key;
    void               *value;
    atm_dict_entry_t   *next;
};


void atm_dict_init();

atm_dict_t *atm_dict_new();
void atm_dict_free(atm_dict_t *e);

atm_dict_entry_t *atm_dict_entry_new (atm_string_t *key, void *value);
void atm_dict_entry_free(atm_dict_entry_t *e);

atm_uint_t atm_dict_key_gen(atm_dict_t *hash, atm_string_t *key);
atm_bool_t atm_dict_contains(atm_dict_t *hash, atm_string_t *key);
void *atm_dict_get(atm_dict_t *hash, atm_string_t *key);
atm_bool_t atm_dict_set(atm_dict_t *hash, atm_string_t *key, void *value);
void atm_dict_remove(atm_dict_t *hash, atm_string_t *key);


#endif /* _ATM_DICT_H_INCLUDED_ */
