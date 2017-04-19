#include <atm_core.h>


static atm_dict_entry_t * atm_dict_entry_new(void *key, void *value);
static atm_uint_t atm_dict_hash_key_gen(atm_dict_t *dict, void *key);
static atm_bool_t atm_dict_entry_match(void *entry, void *key); 
static atm_dict_entry_t *atm_dict_get_entry(atm_dict_t *dict, void *key); 
static void atm_dict_entry_free(void *e);


static atm_list_T_t ATM_DICT_LIST_T = {
    atm_dict_entry_match,
    atm_dict_entry_free,
};


void atm_dict_init()
{
    atm_siphash_init();
}


atm_dict_t *atm_dict_new(atm_dict_T_t *type) 
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


void atm_dict_free(void *d) 
{
    atm_dict_t *dict = (atm_dict_t *) d;
    atm_uint_t i = 0;
    atm_list_t *lptr;
    for (;i<dict->bucket_size; ++i) {
        lptr = dict->bucket[i];
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
    atm_free(dict->bucket);
    atm_free(dict);
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


void atm_dict_entry_free(void *e) 
{
    atm_dict_entry_t *entry = (atm_dict_entry_t *)e;
    atm_dict_t *dict = entry->dict; 
    atm_dict_T_t *type = dict->type;
    if (dict->deep_free) {
        type->free_key(entry->key);
        type->free_value(entry->value);
    }
    atm_free(entry);
}


atm_bool_t atm_dict_entry_match(void *entry, void *key)
{
    atm_bool_t result = ATM_FALSE;
    atm_dict_entry_t *e = (atm_dict_entry_t *)entry;
    atm_dict_T_t *dict_type = NULL;

    if (e == NULL && key == NULL) {
        result = ATM_TRUE;
    } else
    if (e != NULL && key == NULL) {
        result = ATM_FALSE;
    } else {
        dict_type = e->dict->type;
        result = dict_type->match(e->key, key);
    }
    return result;
}


atm_uint_t atm_dict_hash_key_gen(atm_dict_t *dict, void *k) 
{
    atm_string_t *key = (atm_string_t *) k;
    uint64_t hash_key;
    atm_uint_t index = 0;
    hash_key = atm_siphash(key->str, key->len);
    index = hash_key % dict->bucket_size;
    return index;
}


static atm_dict_entry_t *atm_dict_get_entry(atm_dict_t *dict, void *key) 
{
    atm_dict_entry_t *result = NULL;
    atm_uint_t hash_key = atm_dict_hash_key_gen(dict, key);
    atm_list_t *lptr = dict->bucket[hash_key];
    result = (atm_dict_entry_t *) atm_list_find(lptr, key);
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
            lptr = atm_list_new(&ATM_DICT_LIST_T);
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


void atm_dict_remove(atm_dict_t *dict, void *key) 
{
    atm_list_t *lptr;
    atm_dict_entry_t *entry = atm_dict_get_entry(dict, key);
    if (entry != NULL) {
        lptr = entry->list;
        atm_list_del(lptr, entry);
    }
}
