#include <atm_config.h>
#include <atm_core.h>


void atm_hash_init() {
    atm_siphash_init();
}


atm_int_t atm_hash_contains(atm_string_t *key) {
    return 0;
}


atm_uint_t atm_hash_key_func(atm_string_t *key) {
    uint64_t hash;
    hash = atm_siphash(key->str, key->len);
    return (atm_uint_t) hash;
}


void *atm_hash_get(atm_hash_t *hash, atm_string_t *key) {
    return (void *)NULL;
}


void *atm_hash_set(
        atm_hash_t *hash, atm_string_t *key, atm_string_t *value) {
    return (void *)NULL;
}


atm_int_t atm_hash_remove(atm_hash_t *hash, atm_string_t *key) {
    return 0;
}
