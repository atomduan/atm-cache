#include <atm_core.h>


static atm_dict_entry_t * atm_dict_entry_new(void *key, void *value);
static atm_uint_t atm_dict_hash_key_gen(atm_dict_t *dict, void *key);
static atm_bool_t atm_dict_entry_match(void *entry, void *key); 
static atm_dict_entry_t *atm_dict_get_entry(atm_dict_t *dict, void *key); 
static void atm_dict_entry_free(void *entry);


static atm_T_t ATM_DICT_ENTRY_T = {
    atm_dict_entry_match,
    atm_dict_entry_free,
    NULL,
    NULL,
    NULL,
};


/* 
 * FUNCTIONS BIGIN
 * */


void atm_dict_init()
{
    /* TODO */
}


atm_dict_t *atm_dict_new(atm_T_t *type) 
{
    atm_uint_t bsz = ATM_DICT_INITIAL_BUCKET_SIZE;
    atm_dict_t *dict = (atm_dict_t *) atm_malloc(sizeof(atm_dict_t));
    dict->deep_free = ATM_TRUE;
    dict->bucket = (atm_list_t **) atm_malloc(sizeof(atm_list_t *) * bsz);
    dict->bucket_size = bsz;
    dict->size = 0;
    dict->type = type;
    return dict;
}


void atm_dict_free(void *dict) 
{
    atm_dict_t *d = (atm_dict_t *) dict;
    atm_uint_t i = 0;
    atm_list_t *lptr;
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


atm_dict_entry_t * atm_dict_entry_new(void *key, void *value) 
{
    atm_dict_entry_t *result = NULL;
    result = atm_malloc(sizeof(atm_dict_entry_t));
    result->dict = NULL;
    result->key = key; 
    result->value = value; 
    return result;
}


void atm_dict_entry_free(void *entry) 
{
    atm_dict_entry_t *e;
    atm_dict_t *dict;
    atm_T_t *type;
    if (entry != NULL) {
        e = (atm_dict_entry_t *)entry;
        dict = e->dict; 
        if (dict != NULL) {
            type = dict->type;
            if (dict->deep_free) {
                type->free_key(e->key);
                type->free_value(e->value);
            }
        }
        atm_free(e);
    }
}


atm_bool_t atm_dict_entry_match(void *entry, void *hint)
{
    atm_bool_t result = ATM_FALSE;
    atm_dict_entry_t *e = (atm_dict_entry_t *)entry;
    atm_dict_entry_t *h = (atm_dict_entry_t *)hint;
    atm_T_t *type = NULL;

    if (e == NULL || hint == NULL) {
        atm_log_routine(ATM_LOG_ERROR, 
                "atm_dict_entry_match:"
                "entry carrupted should not NULL");
        exit(ATM_ERROR);
    } else {
        type = e->dict->type;
        result = type->match(e->key, h->key);
    }
    return result;
}


atm_uint_t atm_dict_hash_key_gen(atm_dict_t *dict, void *k) 
{
    atm_uint_t result = 0;
    uint64_t hash_key;
    atm_T_t *type = dict->type;
    hash_key = type->hash_key(k);
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


atm_bool_t atm_dict_contains(atm_dict_t *dict, void *key)
{
    atm_dict_entry_t *entry = atm_dict_get_entry(dict, key);
    return entry==NULL? ATM_FALSE : ATM_TRUE;
}


void * atm_dict_get(atm_dict_t *dict, void *key) 
{
    void * result = NULL;
    atm_dict_entry_t *entry = atm_dict_get_entry(dict, key);
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

    atm_dict_entry_t *entry = atm_dict_get_entry(dict, key);

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
    atm_dict_entry_t *entry = atm_dict_get_entry(dict, key);
    if (entry != NULL) {
        lptr = entry->list;
        atm_list_del(lptr, entry);
    }
}
