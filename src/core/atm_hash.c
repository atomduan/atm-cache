#include <atm_config.h>
#include <atm_core.h>


void atm_hash_init() {
    atm_siphash_init();
}


atm_hash_t *atm_hash_new() {
    atm_hash_t *hash = (atm_hash_t *) atm_malloc(sizeof(atm_hash_t));
    hash->table = (atm_hash_buck_t *) atm_malloc(sizeof(atm_hash_buck_t));
    hash->size = ATM_HASH_INITIAL_SIZE;
    return hash;
}


atm_uint_t atm_hash_key_gen(atm_hash_t *hash, atm_string_t *key) {
    atm_uint_t index = 0;
    uint64_t key_hash;
    key_hash = atm_siphash(key->str, key->len);
    index = key_hash % hash->size;
    return index;
}


static atm_hash_entry_t *atm_hash_get_entry(
        atm_hash_t *hash, atm_string_t *key) {
    atm_hash_entry_t *result = NULL;
    atm_hash_entry_t *curr; // what is the initial value ??!!!
    atm_uint_t hash_key = atm_hash_key_gen(hash, key);
    atm_hash_entry_t *table_entry = hash->table[hash_key];
    curr = table_entry;
    do {
        if (atm_string_equal(curr->key, key)) {
            result = curr;
            break;
        }
    } while ((curr=curr->next) != NULL);
    return result;
}


atm_bool_t atm_hash_contains(atm_hash_t *hash, atm_string_t *key) {
    atm_hash_entry_t *entry = atm_hash_get_entry(hash, key);
    return entry==NULL? ATM_FALSE : ATM_TRUE;
}


void *atm_hash_get(atm_hash_t *hash, atm_string_t *key) {
    void * result = NULL;
    atm_hash_entry_t *entry = atm_hash_get_entry(hash, key);
    if (entry != NULL) {
        result = entry->value;
    }
    return result;
}


void atm_hash_entry_free(atm_hash_entry_t* e) {
    atm_string_free(e->key); 
    atm_free(e->value); //TODO: how to do with it!!??
    atm_free(e);
}

void atm_hash_free(atm_hash_t* e) {
    //puke!!!!!!!
}


atm_hash_entry_t *atm_hash_entry_new (atm_string_t *key, void *value) {
    atm_hash_entry_t *result = atm_malloc(sizeof(atm_hash_entry_t));
    result->key = key; 
    result->value = value; 
    result->next = NULL;
    return result;
}


atm_bool_t atm_hash_set(
        atm_hash_t *hash, atm_string_t *key, void *value) {
    atm_bool_t result = ATM_FALSE;
    atm_uint_t hash_key;
    atm_hash_entry_t *table_entry;
    atm_hash_entry_t *curr;

    atm_hash_entry_t *entry = atm_hash_get_entry(hash, key);
    if (entry != NULL) {
        entry->value = value;
        result = ATM_TRUE;
    } else {
        hash_key = atm_hash_key_gen(hash, key);
        table_entry = hash->table[hash_key];
        if (table_entry == NULL) {
            hash->table[hash_key] = atm_hash_entry_new(key,value);   
        } else {
            curr = table_entry;
            while(curr->next != NULL) {
               curr = curr->next; 
            }
            curr->next = atm_hash_entry_new(key,value);
            result = ATM_TRUE;
        }
    }
    return result;
}


void atm_hash_remove(atm_hash_t *hash, atm_string_t *key) {
    atm_uint_t hash_key;
    atm_hash_entry_t *table_entry;
    atm_hash_entry_t *prev = NULL;
    atm_hash_entry_t *curr = NULL;

    hash_key = atm_hash_key_gen(hash, key);
    table_entry = hash->table[hash_key];
    if (table_entry != NULL) {
        curr = table_entry;
        do {
            if (atm_string_equal(curr->key, key)) {
                if (prev == NULL) {
                    hash->table[hash_key] = NULL;
                } else {
                    prev->next = curr->next;
                }
                atm_hash_entry_free(curr);
                break; 
            }
            prev = curr;
            curr = curr->next; 
        } while(curr != NULL);
    }
}
