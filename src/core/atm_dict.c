#include <atm_core.h>
/*
 * Private
 * */
static atm_bool_t
atm_dict_rehash_needed(atm_dict_t *dict);
static void
atm_dict_rehash(atm_dict_t *dict);
static atm_bool_t
atm_dict_table_contains(atm_dict_table_t *table, void *key);
static atm_bool_t
atm_dict_table_del(atm_dict_table_t *table, void *key);
static void *
atm_dict_table_get(atm_dict_table_t *table, void *key);
static void
atm_dict_table_push_entry(atm_dict_table_t *table, atm_dict_entry_t *new_entry);
static void
atm_dict_table_set(atm_dict_table_t *table, void *key, void *val);
static void
atm_dict_table_clear(atm_dict_table_t *table);
static atm_dict_table_t *
atm_dict_table_new(atm_dict_t *dict, atm_uint_t bsz);
static void
atm_dict_table_free(void *table);
static atm_dict_bucket_t *
atm_dict_bucket_new(atm_dict_table_t *table);
static atm_dict_bucket_t *
atm_dict_bucket(atm_dict_table_t *table, void *key);
static void
atm_dict_bucket_free(void *bucket);
static atm_dict_entry_t *
atm_dict_entry(atm_dict_table_t *table, void *key);
static atm_dict_entry_t *
atm_dict_entry_new(void *key, void *val);
static void *
atm_dict_entry_spec(void *entry);
static atm_bool_t
atm_dict_entry_match(void *entry, void *hint);
static uint64_t
atm_dict_entry_hash(void *entry);
static atm_int_t
atm_dict_entry_cmp(void *entry1, void *entry2);
static atm_str_t
atm_dict_entry_str(void *entry);
static void
atm_dict_entry_free(void *entry);
static atm_uint_t
atm_dict_hkey(atm_dict_table_t *table, void *key);
static atm_uint_t
atm_dict_get_size(atm_dict_t *dict);


static atm_T_t ATM_DICT_ENTRY_TYPE = {
    atm_dict_entry_spec,
    atm_dict_entry_match,
    atm_dict_entry_hash,
    atm_dict_entry_cmp,
    atm_dict_entry_str,
    atm_dict_entry_free,
};
static atm_T_t *ATM_DICT_ENTRY_T = &ATM_DICT_ENTRY_TYPE;


atm_uint_t  force_resize_ratio = 5;
atm_uint_t  rehash_loop_time = 1;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_bool_t
atm_dict_rehash_needed(atm_dict_t *dict)
{
    atm_dict_table_t *tb_act;

    tb_act = dict->ht_active;

    if (tb_act == NULL) {
        atm_log_rout(ATM_LOG_FATAL,
            "currapt! dict ht_active can not be NULL");
        exit(ATM_ERROR);
    }

    if (dict != NULL) {
        if (dict->ht_backup != NULL) {
            return ATM_TRUE;
        } else {
            if (dict->enable_resize) {
                if (tb_act->size>=tb_act->bktab_size)
                    return ATM_TRUE;
            } else {
                atm_uint_t ratio = tb_act->size / tb_act->bktab_size;
                if (ratio >= force_resize_ratio)
                    return ATM_TRUE;
            }
        }
    }
    return ATM_FALSE;
}


static void
atm_dict_rehash(atm_dict_t *dict)
{
    atm_dict_table_t *tb_act;
    atm_dict_table_t *tb_tmp;
    atm_dict_bucket_t *bkt;
    atm_uint_t i;
    atm_list_t *bl;
    atm_dict_entry_t *e;

    tb_act = dict->ht_active;

    if(!atm_dict_rehash_needed(dict))
        return;

    if (dict->ht_backup == NULL) {
        if (dict->rehash_index != 0) {
            atm_log_rout(ATM_LOG_FATAL,
                "data currapt rehash_index");
            exit(ATM_ERROR);
        }
        atm_uint_t bsz = tb_act->size*2;
        bsz = atm_util_next_power(bsz);
        dict->ht_backup = atm_dict_table_new(dict, bsz);
    }

    if (dict->rehash_index < tb_act->bktab_size) {
        atm_int_t loop_time = rehash_loop_time;
        while (loop_time--) {
            i = dict->rehash_index;
            if (i == tb_act->bktab_size)
                break;

            if (tb_act->bktab_used == 0) {
                if (tb_act->size != 0) {
                    atm_log_rout(ATM_LOG_FATAL,
                        "data currapt! in rehash end,"
                        "size [%lu]",
                        tb_act->size);
                        exit(ATM_ERROR);
                } else {
                    break;
                }
            }

            if (tb_act->size == 0) {
                if (tb_act->bktab_size != 0) {
                    atm_log_rout(ATM_LOG_FATAL,
                        "data currapt! in rehash end,"
                        "bktab_size [%lu]",
                        tb_act->bktab_size);
                        exit(ATM_ERROR);
                } else {
                    break;
                }
            }

            bkt = tb_act->bktab[i];
            dict->rehash_index++;
            if (bkt == NULL)
                continue;

            bl = bkt->list;
            while ((e=atm_list_lpop(bl)) != NULL) {
                atm_dict_table_push_entry(dict->ht_backup,e);
                tb_act->size--;
            }
            tb_act->bktab_used--;
        }
    } else {
        atm_log_rout(ATM_LOG_FATAL,
            "data currapt! in rehash loop prechecking");
        exit(ATM_ERROR);
    }

    /* check whether rehash can finish */
    if (tb_act->size == 0) {
        if (dict->rehash_index > tb_act->bktab_size) {
            atm_log_rout(ATM_LOG_FATAL,
                "data currapt! rehash_index on rehash finish,"
                "rehash_index[%u], bktab_size[%u]",
                dict->rehash_index,
                tb_act->bktab_size);
            exit(ATM_ERROR);
        } else
        if (tb_act->bktab_used != 0) {
            atm_log_rout(ATM_LOG_FATAL,
                "data currapt! bktab_used on rehash finish,"
                "bktab_used[%u]",tb_act->bktab_used);
            exit(ATM_ERROR);
        } else {
           tb_tmp = dict->ht_active;
           dict->ht_active = dict->ht_backup;
           dict->ht_backup = NULL;
           dict->rehash_index = 0;
           atm_dict_table_free(tb_tmp);
        }
    }
}


static atm_bool_t
atm_dict_table_contains(atm_dict_table_t *table, void *key)
{
    atm_bool_t res = ATM_FALSE;
    atm_dict_entry_t *entry;
    if (table != NULL) {
        entry = atm_dict_entry(table, key);
        res = entry==NULL?ATM_FALSE:ATM_TRUE;
    }
    return res;
}


static atm_bool_t
atm_dict_table_del(atm_dict_table_t *table, void *key)
{
    atm_bool_t res = ATM_FALSE;
    atm_list_t *lptr;
    atm_dict_entry_t *entry;
    atm_dict_bucket_t *bkt;
    atm_dict_table_t *tbl;

    if (table != NULL) {
        tbl = table;
        entry = atm_dict_entry(tbl,key);
        if (entry != NULL) {
            bkt = atm_dict_bucket(tbl,key);
            lptr = bkt->list;
            atm_list_del(lptr,entry);
            atm_dict_entry_free(entry);
            res = ATM_TRUE;
            tbl->size--;
        }
    }
    return res;
}


static void *
atm_dict_table_get(atm_dict_table_t *table, void *key)
{
    void * res = NULL;
    atm_dict_entry_t *entry;

    if (table != NULL) {
        entry = atm_dict_entry(table, key);
        if (entry != NULL) {
            res = entry->val;
        }
    }
    return res;
}


static void
atm_dict_table_push_entry(atm_dict_table_t *table,
        atm_dict_entry_t *new_entry)
{
    atm_dict_bucket_t *bkt;
    atm_list_t *lptr;
    atm_uint_t hash_key;
    atm_dict_table_t *tbl;
    void *key;

    if (table != NULL) {
        tbl = table;
        key = new_entry->key;

        bkt = atm_dict_bucket(tbl,key);
        if (bkt == NULL) {
            bkt = atm_dict_bucket_new(tbl);
            hash_key = atm_dict_hkey(tbl,key);
            tbl->bktab[hash_key] = bkt;
            tbl->bktab_used++;
        }

        lptr = bkt->list;
        atm_list_push(lptr,new_entry);
        tbl->size++;
    }
}


static void
atm_dict_table_set(atm_dict_table_t *table,
        void *key, void *val)
{
    atm_dict_entry_t *entry;
    atm_dict_entry_t *new_entry;
    atm_dict_table_t *tbl;

    if (table != NULL) {
        tbl = table;
        entry = atm_dict_entry(tbl,key);
        if (entry != NULL) {
            entry->val = val;
        } else {
            new_entry = atm_dict_entry_new(key,val);
            new_entry->dict = table->dict;
            atm_dict_table_push_entry(table, new_entry);
        }
    }
}


static void
atm_dict_table_clear(atm_dict_table_t *table)
{
    atm_uint_t i;
    atm_dict_bucket_t *bkt;
    if (table != NULL) {
        for (i=0; i<table->bktab_size; ++i) {
            bkt = table->bktab[i];
            if (bkt != NULL) {
                atm_dict_bucket_free(bkt);
            }
        }
        memset(table->bktab,ATM_MEM_ZERO,
                table->bktab_size);
        table->bktab_used = 0;
        table->size = 0;
    }
}


static atm_dict_table_t *
atm_dict_table_new(atm_dict_t *dict, atm_uint_t bsz)
{
    atm_dict_table_t *res = NULL;
    res = atm_alloc(sizeof(atm_dict_table_t));
    res->bktab = atm_alloc(sizeof(atm_dict_bucket_t *)*bsz);
    res->bktab_size = bsz;
    res->bktab_used = 0;
    res->size = 0;
    res->dict = dict;
    return res;
}


static void
atm_dict_table_free(void *table)
{
    atm_dict_table_t *t;

    if (table != NULL) {
        t = (atm_dict_table_t *)table;
        atm_dict_table_clear(t);
        atm_free(t->bktab);
        atm_free(t);
    }
}


static atm_uint_t
atm_dict_hkey(atm_dict_table_t *table, void *k)
{
    atm_uint_t res = 0;
    uint64_t hash_key;
    atm_T_t *k_t;
    atm_dict_t *dict;

    if (table != NULL) {
        dict = table->dict;
        k_t = dict->k_type;
        hash_key = k_t->hash(k);
        res = hash_key % table->bktab_size;
    }
    return res;
}


static atm_dict_entry_t *
atm_dict_entry(atm_dict_table_t *table, void *key)
{
    atm_dict_entry_t *res = NULL;
    atm_dict_bucket_t *bkt;
    atm_dict_entry_t *hint;
    atm_list_t *list;

    if (table != NULL) {
        bkt = atm_dict_bucket(table, key);
        if (bkt != NULL) {
            list = bkt->list;
            /* create a tmp hint to test */
            hint = atm_dict_entry_new(key, NULL);
            res = (atm_dict_entry_t *)
                atm_list_find(list, hint);
            /* dont forget free hint */
            atm_dict_entry_free(hint);
        }
    }
    return res;
}


static atm_dict_bucket_t *
atm_dict_bucket_new(atm_dict_table_t *table)
{
    atm_dict_bucket_t *res = NULL;
    atm_list_t *lptr;

    res = atm_alloc(sizeof(atm_dict_bucket_t));
    res->table = table;
    res->dict = table->dict;
    /* lptr is part of data structure
     * so need to set shallow free */
    lptr = atm_list_new(ATM_DICT_ENTRY_T,
            ATM_FREE_SHALLOW);
    res->list = lptr;
    return res;
}


static atm_dict_bucket_t *
atm_dict_bucket(atm_dict_table_t *table, void *key)
{
    atm_dict_bucket_t *res = NULL;
    atm_uint_t hash_key = 0;

    if (table != NULL) {
        hash_key = atm_dict_hkey(table,key);
        res = table->bktab[hash_key];
    }
    return res;
}


static void
atm_dict_bucket_free(void *bucket)
{
    atm_dict_bucket_t *bkt;
    atm_list_t *lptr;

    if (bucket != NULL) {
        bkt = (atm_dict_bucket_t *) bucket;
        lptr = bkt->list;
        if (lptr->free_type == ATM_FREE_SHALLOW) {
            atm_list_free(lptr);
        } else {
            atm_log_rout(ATM_LOG_ERROR,
                "bucket inner list curraput");
            exit(ATM_ERROR);
        }
        atm_free(bkt);
    }
}


static atm_dict_entry_t *
atm_dict_entry_new(void *key, void *val)
{
    atm_dict_entry_t *res = NULL;

    res = atm_alloc(sizeof(atm_dict_entry_t));
    res->key = key;
    res->val = val;
    res->dict = NULL;
    return res;
}


static void *
atm_dict_entry_spec(void *entry)
{
    void *res = NULL;
    return res;
}


static atm_bool_t
atm_dict_entry_match(void *entry, void *hint)
{
    atm_bool_t res = ATM_FALSE;
    atm_T_t *k_t;
    atm_dict_entry_t *e;
    atm_dict_entry_t *h;

    e = (atm_dict_entry_t *)entry;
    h = (atm_dict_entry_t *)hint;
    if (e == NULL || hint == NULL) {
        atm_log_rout(ATM_LOG_FATAL,
                "atm_dict_entry_match:"
                "entry corrupted should not NULL");
        exit(ATM_ERROR);
    } else {
        k_t = e->dict->k_type;
        res = k_t->match(e->key, h->key);
    }
    return res;
}


static uint64_t
atm_dict_entry_hash(void *entry)
{
    uint64_t res = 0;
    atm_dict_entry_t *e;
    atm_str_t e_str;
    atm_uint_t e_len;

    e = (atm_dict_entry_t *) entry;
    e_str = atm_str_ptr(e);
    e_len = atm_str_len(e_str);
    res = atm_dict_hash(e_str, e_len);

    atm_str_free(e_str);
    return res;
}


static atm_int_t
atm_dict_entry_cmp(void *entry1, void *entry2)
{
    return ATM_CMP_EQ;
}


static atm_str_t
atm_dict_entry_str(void *entry)
{
    atm_str_t res = NULL;
    atm_str_t ks = NULL;
    atm_str_t vs = NULL;
    atm_T_t *k_t;
    atm_T_t *v_t;
    atm_dict_entry_t *e;

    e = (atm_dict_entry_t *) entry;
    if (e != NULL) {
        if (e->dict != NULL) {
            k_t = e->dict->k_type;
            if (k_t && k_t->str) {
                ks = k_t->str(e->key);
            } else {
                ks = atm_str_ptr(e->key);
            }
            v_t = e->dict->v_type;
            if (v_t && v_t->str) {
                vs = v_t->str(e->val);
            } else {
                vs = atm_str_ptr(e->val);
            }
        }
    }
    res = atm_str_cat_dup(ks,vs);
    atm_str_free(ks);
    atm_str_free(vs);
    return res;
}


static void
atm_dict_entry_free(void *entry)
{
    atm_dict_entry_t *e;
    atm_dict_t *dict;

    if (entry != NULL) {
        e = (atm_dict_entry_t *)entry;
        dict = e->dict;
        if (dict != NULL) {
            if (dict->free_type == ATM_FREE_DEEP) {
                if (e->key && dict->k_type
                        && dict->k_type->free) {
                    dict->k_type->free(e->key);
                }
                if (e->val && dict->v_type
                        && dict->v_type->free) {
                    dict->v_type->free(e->val);
                }
            }
        }
        atm_free(e);
    }
}


static atm_uint_t
atm_dict_get_size(atm_dict_t *dict)
{
    atm_uint_t res = 0;
    atm_dict_table_t *t;
    if (dict != NULL) {
        t = dict->ht_active;
        if (t!=NULL) res += t->size;
        t = dict->ht_backup;
        if (t!=NULL) res += t->size;
    }
    return res;
}


/*
 * Public
 * */
void
atm_dict_init()
{
    atm_siphash_init();
}


atm_dict_t *
atm_dict_new(atm_T_t *k_type, atm_T_t *v_type,
        atm_uint_t f_type)
{
    atm_uint_t bsz = ATM_DICT_INITIAL_BUCKET_SIZE;
    atm_dict_t *res;
    res = atm_alloc(sizeof(atm_dict_t));
    res->free_type = f_type;
    res->k_type = k_type;
    res->v_type = v_type;
    res->ht_active = atm_dict_table_new(res,bsz);
    res->ht_backup = NULL;
    res->enable_resize = ATM_TRUE;
    res->rehash_index = 0;
    pthread_rwlock_init(&res->rwlk,NULL);
    return res;
}


atm_str_t
atm_dict_str(void *dict)
{
    atm_str_t res = NULL;
    atm_dict_t *d;
    atm_uint_t dict_size = 0;

    if (dict != NULL) {
        d = (atm_dict_t *) dict;
        pthread_rwlock_rdlock(&d->rwlk);
        dict_size = atm_dict_get_size(dict);
        res = atm_str_fmt(
            "free_type[%d];"
            "active_bucket_size[%ld];"
            "backup_bucket_size[%ld];"
            "size[%ld];addr[%p]",
            d->free_type,
            d->ht_active->bktab_size,
            d->ht_backup->bktab_size,
            dict_size,d);
        pthread_rwlock_unlock(&d->rwlk);
    } else {
        res = atm_str_new("NULL");
    }
    return res;
}


void
atm_dict_free(void *dict)
{
    /* TODO how free this obj safely in multithread condition
     * a safely free enviroment should be provided by the caller's!*/
    atm_dict_t *d;
    atm_dict_table_t *table;

    if (dict != NULL) {
        d = (atm_dict_t *) dict;
        table = d->ht_active;
        if (table != NULL)
            atm_dict_table_free(table);
        table = d->ht_backup;
        if (table != NULL)
            atm_dict_table_free(table);
        pthread_rwlock_destroy(&d->rwlk);
        atm_free(d);
    }
}


atm_bool_t
atm_dict_contains(atm_dict_t *dict, void *key)
{
    atm_bool_t res = ATM_FALSE;
    atm_dict_table_t *table;
    if (dict != NULL) {
        pthread_rwlock_rdlock(&dict->rwlk);
        table = dict->ht_active;
        res = atm_dict_table_contains(table,key);
        if (res == ATM_FALSE) {
            table = dict->ht_backup;
            res = atm_dict_table_contains(table,key);
        }
        pthread_rwlock_unlock(&dict->rwlk);
    }
    return res;
}


void *
atm_dict_get(atm_dict_t *dict, void *key)
{
    void * res = NULL;
    atm_dict_table_t *table;
    if (dict != NULL) {
        pthread_rwlock_rdlock(&dict->rwlk);
        table = dict->ht_active;
        res = atm_dict_table_get(table,key);
        if (res == NULL) {
            table = dict->ht_backup;
            res = atm_dict_table_get(table,key);
        }
        pthread_rwlock_unlock(&dict->rwlk);
    }
    return res;
}


void
atm_dict_set(atm_dict_t *dict, void *key, void *val)
{
    atm_dict_table_t *table;

    if (dict != NULL) {
        pthread_rwlock_wrlock(&dict->rwlk);
        if (dict->ht_backup != NULL)
            table = dict->ht_backup;
        else
            table = dict->ht_active;

        atm_dict_table_set(table,key,val);
        atm_dict_rehash(dict);
        pthread_rwlock_unlock(&dict->rwlk);
    }
}


void
atm_dict_del(atm_dict_t *dict, void *key)
{
    atm_bool_t res = ATM_FALSE;
    atm_dict_table_t *table;

    if (dict != NULL) {
        pthread_rwlock_wrlock(&dict->rwlk);
        table = dict->ht_active;
        res = atm_dict_table_del(table,key);
        if (res == ATM_FALSE) {
            table = dict->ht_backup;
            if (table != NULL)
                res = atm_dict_table_del(table,key);
        }
        atm_dict_rehash(dict);
        pthread_rwlock_unlock(&dict->rwlk);
    }
}


uint64_t
atm_dict_hash(char *input, atm_uint_t inlen)
{
    return atm_siphash(input, inlen);
}


uint64_t
atm_dict_hash_nocase(char *input, atm_uint_t inlen)
{
    return atm_siphash_nocase(input, inlen);
}


void
atm_dict_clear(atm_dict_t *dict)
{
    atm_dict_t *d;

    if (dict != NULL) {
        pthread_rwlock_wrlock(&dict->rwlk);
        d = dict;
        /* clear all data */
        atm_dict_table_clear(d->ht_active);
        atm_dict_table_clear(d->ht_backup);
        /* free the backup */
        atm_dict_table_free(d->ht_backup);
        d->ht_backup = NULL;
        d->rehash_index = 0;
        pthread_rwlock_unlock(&dict->rwlk);
    }
}


atm_uint_t
atm_dict_size(atm_dict_t *dict)
{
    return atm_dict_get_size(dict);
}


void
atm_dict_resize_enable(atm_dict_t *dict)
{
    pthread_rwlock_wrlock(&dict->rwlk);
    dict->enable_resize = ATM_TRUE;
    pthread_rwlock_unlock(&dict->rwlk);
}


void
atm_dict_resize_disable(atm_dict_t *dict)
{
    pthread_rwlock_wrlock(&dict->rwlk);
    dict->enable_resize = ATM_FALSE;
    pthread_rwlock_unlock(&dict->rwlk);
}
