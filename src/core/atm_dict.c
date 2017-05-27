#include <atm_core.h>
/*
 * Private
 * */
static void
atm_dict_rehash(atm_dict_t *dict);

static atm_bool_t
atm_dict_table_contains(atm_dict_table_t *table, void *key);
static atm_bool_t
atm_dict_table_del(atm_dict_table_t *table, void *key);
static void *
atm_dict_table_get(atm_dict_table_t *table, void *key);
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


static atm_T_t ATM_DICT_ENTRY_TYPE = {
    atm_dict_entry_spec,
    atm_dict_entry_match,
    atm_dict_entry_hash,
    atm_dict_entry_cmp,
    atm_dict_entry_str,
    atm_dict_entry_free,
};
static atm_T_t *ATM_DICT_ENTRY_T = &ATM_DICT_ENTRY_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_dict_rehash(atm_dict_t *dict)
{

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
atm_dict_table_set(atm_dict_table_t *table,
        void *key, void *val)
{
    atm_dict_entry_t *entry;
    atm_dict_bucket_t *bkt;
    atm_list_t *lptr;
    atm_dict_entry_t *new_entry;
    atm_uint_t hash_key;
    atm_dict_table_t *tbl;
    atm_dict_t *dict;

    if (table != NULL) {
        tbl = table;
        dict = table->dict;
        entry = atm_dict_entry(tbl,key);
        if (entry != NULL) {
            entry->val = val;
        } else {
            bkt = atm_dict_bucket(tbl,key);
            if (bkt == NULL) {
                bkt = atm_dict_bucket_new(tbl);
                hash_key = atm_dict_hkey(tbl,key);
                tbl->bktab[hash_key] = bkt;
                tbl->bktab_used++;
            }
            lptr = bkt->list;
            new_entry = atm_dict_entry_new(key,val);
            new_entry->dict = dict;
            atm_list_push(lptr,new_entry);
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
    res->dict = dict;
    return res;
}


static void
atm_dict_table_free(void *table)
{
    atm_dict_table_t *t;

    if (table != NULL) {
        t = (atm_dict_table_t *)table;
        if (t->bktab_used > 0)
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
                "entry carrupted should not NULL");
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
    res = atm_str_cat(ks,vs);
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
    res->size = 0;
    res->k_type = k_type;
    res->v_type = v_type;
    res->ht_active = atm_dict_table_new(res,bsz);
    res->ht_backup = NULL;
    return res;
}


atm_str_t
atm_dict_str(void *dict)
{
    atm_str_t res = NULL;
    atm_dict_t *d;

    d = (atm_dict_t *) dict;
    if (d != NULL) {
        res = atm_str_fmt(
            "free_type[%d];"
            "active_bucket_size[%ld];"
            "backup_bucket_size[%ld];"
            "size[%ld];addr[%p]",
            d->free_type,
            d->ht_active->bktab_size,
            d->ht_backup->bktab_size,
            d->size,d);
    } else {
        res = atm_str_new("NULL");
    }
    return res;
}


void
atm_dict_free(void *dict)
{
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
        atm_free(d);
    }
}


atm_bool_t
atm_dict_contains(atm_dict_t *dict, void *key)
{
    atm_bool_t res = ATM_FALSE;
    atm_dict_table_t *table;
    if (dict != NULL) {
        table = dict->ht_active;
        res = atm_dict_table_contains(table,key);
        if (res == ATM_TRUE) return res;
        table = dict->ht_backup;
        res = atm_dict_table_contains(table,key);
    }
    return res;
}


void *
atm_dict_get(atm_dict_t *dict, void *key)
{
    void * res = NULL;
    atm_dict_table_t *table;
    if (dict != NULL) {
        table = dict->ht_active;
        res = atm_dict_table_get(table,key);
        if (res != NULL) return res;
        table = dict->ht_backup;
        res = atm_dict_table_get(table,key);
    }
    return res;
}


void
atm_dict_set(atm_dict_t *dict, void *key, void *val)
{
    atm_dict_table_t *table;
    
    if (dict != NULL) {
        if (dict->ht_backup != NULL)
            table = dict->ht_backup;
        else
            table = dict->ht_active;

        atm_dict_table_set(table,key,val);
        dict->size++;
        atm_dict_rehash(dict);
    }
}


void
atm_dict_del(atm_dict_t *dict, void *key)
{
    atm_bool_t res = ATM_FALSE;
    atm_dict_table_t *table;

    table = dict->ht_active;
    res = atm_dict_table_del(table,key);
    if (res == ATM_FALSE) {
        table = dict->ht_backup;
        if (table != NULL)
            res = atm_dict_table_del(table,key);
    }

    if (res == ATM_TRUE)
        dict->size--;
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
        d = dict;
        /* clear all data */
        atm_dict_table_clear(d->ht_active);
        atm_dict_table_clear(d->ht_backup);
        /* free the backup */
        atm_dict_table_free(d->ht_backup);
        d->ht_backup = NULL; 
        d->size = 0;
    }
}
