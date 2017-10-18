#ifndef _ATM_DICT_H_INCLUDED_
#define _ATM_DICT_H_INCLUDED_

#include <atm_core.h>

#define ATM_DICT_INITIAL_BUCKET_SIZE 256


typedef struct atm_dict_entry_s     atm_dict_entry_t;
typedef struct atm_dict_bucket_s    atm_dict_bucket_t;
typedef struct atm_dict_table_s     atm_dict_table_t;


struct atm_dict_s {
    atm_uint_t              free_type;
    atm_dict_table_t       *ht_active;
    atm_dict_table_t       *ht_backup;
    atm_T_t                *k_type;
    atm_T_t                *v_type;
    atm_bool_t              enable_resize;
    atm_uint_t              rehash_index;

    pthread_rwlock_t        rwlk;
};


struct atm_dict_table_s {
    atm_dict_t             *dict;
    atm_dict_bucket_t     **bktab; //TODO, need const
    atm_uint_t              bktab_size;
    atm_uint_t              bktab_used;
    atm_uint_t              size;
};


struct atm_dict_bucket_s {
    atm_dict_t             *dict;
    atm_dict_table_t       *table;
    /* inner list contain dict_entrys */
    atm_list_t             *list;
};


struct atm_dict_entry_s {
    atm_dict_t  *dict;
    void   *key;
    void   *val;
};


/* public dict type lifecycle */
void
atm_dict_init();

atm_dict_t *
atm_dict_new(atm_T_t *k_type, atm_T_t *v_type, atm_uint_t free_type);

atm_str_t
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

uint64_t
atm_dict_hash(char *input, atm_uint_t inlen);

uint64_t
atm_dict_hash_nocase(char *input, atm_uint_t inlen);

void
atm_dict_clear(atm_dict_t *dict);

atm_uint_t
atm_dict_size(atm_dict_t *dict);

void
atm_dict_resize_enable(atm_dict_t *dict);

void
atm_dict_resize_disable(atm_dict_t *dict);


#endif /* _ATM_DICT_H_INCLUDED_ */
