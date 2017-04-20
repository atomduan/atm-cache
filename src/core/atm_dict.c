#include <atm_core.h>


/*
 * Private
 * */


/* entry type lifecycle */
static atm_dict_entry_t * atm_dict_entry_new(void *key, void *value);
static void *atm_dict_entry_spec(void *entry);
static atm_bool_t atm_dict_entry_match(void *entry, void *hint); 
static uint64_t atm_dict_entry_hash(void *entry);
static atm_int_t atm_dict_entry_compare(void *entry1, void *entry2);
static atm_string_t *atm_dict_entry_string(void *entry);
static void atm_dict_entry_free(void *entry);

static atm_T_t ATM_DICT_ENTRY_T = {
    atm_dict_entry_spec,
    atm_dict_entry_match,
    atm_dict_entry_hash,
    atm_dict_entry_compare,
    atm_dict_entry_string,
    atm_dict_entry_free,
};


/* funcs */
static atm_uint_t atm_dict_hash_key_gen(atm_dict_t *dict, void *key);
static atm_dict_entry_t *atm_dict_get_entry(atm_dict_t *dict, void *key); 


/*
 * Public
 * */


atm_T_t ATM_DICT_T = {
    atm_dict_spec,
    atm_dict_match,
    atm_dict_hash,
    atm_dict_compare,
    atm_dict_string,
    atm_dict_free,
};


/* ---------------------IMPLEMENTATIONS--------------------------- */


/*
 * Private
 * */


static atm_dict_entry_t * atm_dict_entry_new(void *key, void *value) 
{
    atm_dict_entry_t *result = NULL;
    result = atm_malloc(sizeof(atm_dict_entry_t));
    result->dict = NULL;
    result->key = key; 
    result->value = value; 
    return result;
}


static void *atm_dict_entry_spec(void *entry)
{
    void *result = NULL;
    return result;
}


static atm_bool_t atm_dict_entry_match(void *entry, void *hint)
{
    atm_bool_t result = ATM_FALSE;
    atm_T_t *k_t;
    atm_dict_entry_t *e;
    atm_dict_entry_t *h;

    e = (atm_dict_entry_t *)entry;
    h = (atm_dict_entry_t *)hint;
    if (e == NULL || hint == NULL) {
        atm_log_routine(ATM_LOG_ERROR, 
                "atm_dict_entry_match:"
                "entry carrupted should not NULL");
        exit(ATM_ERROR);
    } else {
        k_t = e->dict->k_type;
        result = k_t->match(e->key, h->key);
    }
    return result;
}


static uint64_t atm_dict_entry_hash(void *entry)
{
    uint64_t result;
    atm_dict_entry_t *e; 
    atm_string_t *e_str;

    e = (atm_dict_entry_t *) entry;
    e_str = atm_string_ptr_tostr(e); 
    result = atm_siphash(e_str->str, e_str->len);

    atm_string_free(e_str);
    return result;
}


static atm_int_t atm_dict_entry_compare(void *entry1, void *entry2)
{
    return ATM_COMP_EQUAL;
}


static atm_string_t *atm_dict_entry_string(void *entry)
{
    atm_string_t *result = NULL;
    atm_string_t *ks = NULL;
    atm_string_t *vs = NULL;
    atm_T_t *k_t = NULL;
    atm_T_t *v_t = NULL;
    atm_dict_entry_t *e = NULL;

    e = (atm_dict_entry_t *) entry;
    if (e != NULL) {
        if (e->dict != NULL) {
            k_t = e->dict->k_type;
            if (k_t != NULL && k_t->string != NULL) {
                ks = k_t->string(e->key);
            } else {
                ks = atm_string_ptr_tostr(e->key);
            }
            if (v_t != NULL && v_t->string != NULL) {
                vs = v_t->string(e->value);
            } else {
                vs = atm_string_ptr_tostr(e->value);
            }
        }
    }
    result = atm_string_fmt("key[%s];value[%s];",ks,vs);

    atm_string_free(ks);
    atm_string_free(vs);
    return result;
}


static void atm_dict_entry_free(void *entry) 
{
    atm_dict_entry_t *e;
    atm_dict_t *dict;
    if (entry != NULL) {
        e = (atm_dict_entry_t *)entry;
        dict = e->dict; 
        if (dict != NULL) {
            if (dict->deep_free) {
                dict->k_type->free(e->key);
                dict->v_type->free(e->value);
            }
        }
        atm_free(e);
    }
}


static atm_uint_t atm_dict_hash_key_gen(atm_dict_t *dict, void *k) 
{
    atm_uint_t result = 0;
    uint64_t hash_key;
    atm_T_t *k_t;

    k_t = dict->k_type;
    hash_key = k_t->hash(k);
    result = hash_key % dict->bucket_size;
    return result;
}


static atm_dict_entry_t *atm_dict_get_entry(atm_dict_t *dict, void *key) 
{
    atm_dict_entry_t *result = NULL;
    atm_dict_entry_t *hint = NULL;
    atm_uint_t hash_key;
    atm_list_t *lptr;

    hash_key = atm_dict_hash_key_gen(dict, key);
    lptr = dict->bucket[hash_key];
    /* create a tmp hint to test */
    hint = atm_dict_entry_new(key, NULL);
    result = (atm_dict_entry_t *) atm_list_find(lptr, hint);
    /* dont forget free hint */
    atm_dict_entry_free(hint);
    return result;
}


/*
 * Public
 * */


void atm_dict_init()
{
    /* TODO */
}


atm_dict_t *atm_dict_new(atm_T_t *k_type, atm_T_t *v_type) 
{
    atm_uint_t  bsz;
    atm_dict_t *dict;

    bsz = ATM_DICT_INITIAL_BUCKET_SIZE;
    dict = (atm_dict_t *) atm_malloc(sizeof(atm_dict_t));
    dict->deep_free = ATM_TRUE;
    dict->bucket = (atm_list_t **) atm_malloc(sizeof(atm_list_t *) * bsz);
    dict->bucket_size = bsz;
    dict->size = 0;
    dict->k_type = k_type;
    dict->v_type = v_type;

    return dict;
}


void *atm_dict_spec(void *dict)
{
    void *result = NULL;
    return result;
}


atm_bool_t atm_dict_match(void *dict1, void *dict2)
{
    atm_bool_t result = ATM_FALSE;
    if (dict1 == dict2) {
        result = ATM_TRUE;
    }
    return result;
}


uint64_t atm_dict_hash(void *dict)
{
    uint64_t result;
    atm_dict_t *d; 
    atm_string_t *d_str;

    d = (atm_dict_t *) dict;
    d_str = atm_string_ptr_tostr(d); 
    result = atm_siphash(d_str->str, d_str->len);

    atm_string_free(d_str);
    return result;
}


atm_int_t atm_dict_compare(void *dict1, void *dict2)
{
    return ATM_COMP_EQUAL;
}


atm_string_t *atm_dict_string(void *dict)
{
    atm_string_t *result;
    atm_dict_t *d; 

    d = (atm_dict_t *) dict;
    if (d != NULL) {
        result = atm_string_fmt(
                "deep_free[%d];bucket_size[%ld];size[%ld]",
                d->deep_free,
                d->bucket_size,
                d->size);
    } else {
        result = atm_string_new("NULL");
    }
    return result;
}


void atm_dict_free(void *dict) 
{
    atm_dict_t *d;
    atm_uint_t  i = 0;
    atm_list_t *lptr;

    d = (atm_dict_t *) dict;
    for (;i<d->bucket_size; ++i) {
        lptr = d->bucket[i];
        if (lptr->deep_free) {
            /*
             * containers inner container must be free_deep
             * or else the top container's free can not triger
             * the basic value's free action
             */
            atm_list_free(lptr);
        } else {
            atm_log_routine(ATM_LOG_ERROR, "dict inner list curraput");
            exit(ATM_ERROR);
        }
    }

    atm_free(d->bucket);
    atm_free(d);
}


atm_bool_t atm_dict_contains(atm_dict_t *dict, void *key)
{
    atm_dict_entry_t *entry;
    entry = atm_dict_get_entry(dict, key);
    return entry==NULL? ATM_FALSE : ATM_TRUE;
}


void * atm_dict_get(atm_dict_t *dict, void *key) 
{
    void * result = NULL;
    atm_dict_entry_t *entry;

    entry = atm_dict_get_entry(dict, key);
    if (entry != NULL) {
        result = entry->value;
    }
    return result;
}


void atm_dict_set(atm_dict_t *dict, void *key, void *value)
{
    atm_uint_t hash_key;
    atm_list_t *lptr = NULL; 
    atm_dict_entry_t *new_entry = NULL;
    atm_dict_entry_t *entry;
    
    entry = atm_dict_get_entry(dict, key);
    if (entry != NULL) {
        entry->value = value;
    } else {
        hash_key = atm_dict_hash_key_gen(dict, key);
        lptr = dict->bucket[hash_key];
        if (lptr == NULL) {
            lptr = atm_list_new(&ATM_DICT_ENTRY_T);
            /* lptr is part of data structure so need to set deep free */
            lptr->deep_free = ATM_TRUE;
            dict->bucket[hash_key] = lptr;
        }
        new_entry = atm_dict_entry_new(key,value);   
        new_entry->dict = dict;
        new_entry->list = lptr;
        atm_list_push(lptr, new_entry);
    }
}


void atm_dict_del(atm_dict_t *dict, void *key) 
{
    atm_list_t *lptr;
    atm_dict_entry_t *entry;

    entry = atm_dict_get_entry(dict, key);
    if (entry != NULL) {
        lptr = entry->list;
        atm_list_del(lptr, entry);
    }
}
