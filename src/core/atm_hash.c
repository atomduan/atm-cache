#include <atm_config.h>
#include <atm_core.h>

static uint8_t siphash_seed[16];

void atm_hash_init_siphash_seed() {
    char *p = (char *)siphash_seed;
    unsigned int len = sizeof(siphash_seed);
    char *charset = "0123456789abcdef";
    unsigned int j;

    char *x = p;
    unsigned int l = len;
    struct timeval tv;
    pid_t pid = getpid();
    gettimeofday(&tv,NULL);
    if (l >= sizeof(tv.tv_usec)) {
        memcpy(x,&tv.tv_usec,sizeof(tv.tv_usec));
        l -= sizeof(tv.tv_usec);
        x += sizeof(tv.tv_usec);
    }
    if (l >= sizeof(tv.tv_sec)) {
        memcpy(x,&tv.tv_sec,sizeof(tv.tv_sec));
        l -= sizeof(tv.tv_sec);
        x += sizeof(tv.tv_sec);
    }
    if (l >= sizeof(pid)) {
        memcpy(x,&pid,sizeof(pid));
        l -= sizeof(pid);
        x += sizeof(pid);
    }
    for (j = 0; j < len; j++) {
        p[j] ^= rand();
        p[j] = charset[p[j] & 0x0F];
    }
}

atm_hash_t *atm_hash_init() {
    atm_hash_t *hash = NULL;
    hash = atm_malloc(sizeof(atm_hash_t));
    return hash;
}

atm_int_t atm_hash_contains(atm_string_t *key) {
    return 0;
}

atm_uint_t atm_hash_key_func(atm_string_t *key) {
    uint64_t hash;
    uint8_t *hash_seed;
    hash_seed = siphash_seed;
    hash = atm_siphash(key->data, key->len, hash_seed);
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
