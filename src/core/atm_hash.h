#ifndef _ATM_HASH_H_INCLUDED_
#define _ATM_HASH_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>


#define ATM_HASH_INITIAL_SIZE 256


typedef struct atm_hash_entry_s atm_hash_entry_t;
struct atm_hash_entry_s {
    atm_string_t *key;
    void *value;
    atm_hash_entry_t *next;
};


typedef atm_hash_entry_t *      atm_hash_buck_t;


typedef struct atm_hash_s atm_hash_t;
struct atm_hash_s {
    atm_hash_buck_t *table;
    atm_uint_t size;
};


void atm_hash_init();

atm_hash_entry_t *atm_hash_entry_new (atm_string_t *key, void *value);
void atm_hash_entry_free(atm_hash_entry_t* e);

atm_hash_t *atm_hash_new();
void atm_hash_free(atm_hash_t* e);

atm_uint_t atm_hash_key_gen(atm_hash_t *hash, atm_string_t *key);
atm_bool_t atm_hash_contains(atm_hash_t *hash, atm_string_t *key);
void *atm_hash_get(atm_hash_t *hash, atm_string_t *key);
atm_bool_t atm_hash_set(atm_hash_t *hash, atm_string_t *key, void *value);
void atm_hash_remove(atm_hash_t *hash, atm_string_t *key);


#endif
