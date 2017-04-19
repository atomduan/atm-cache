#include <atm_core.h>


static void atm_list_entry_isol(atm_list_entry_t * e);
static atm_list_entry_t *atm_list_find_linear(atm_list_t *list, void *key);
static atm_list_entry_t *atm_list_lpop_entry(atm_list_t *list);
static atm_list_entry_t *atm_list_rpop_entry(atm_list_t *list);
static atm_list_entry_t *atm_list_entry_new(void *value);
static void atm_list_entry_free(void *e);


/* make sure that the entry has no deps with the list position */
static void atm_list_entry_isol(atm_list_entry_t * e)
{
    if (e != NULL) {
        e->prev = NULL;
        e->next = NULL;
    }
}


void atm_list_init()
{
    /*TODO*/
}


atm_list_t *atm_list_new(atm_list_T_t *type)
{
    atm_list_t *result = NULL;
    result = (atm_list_t *) atm_malloc(sizeof(atm_list_t));
    result->head = NULL;
    result->tail = NULL;
    result->size = 0;
    result->type = type;
    result->deep_free = ATM_FALSE;
    return result;
}


void atm_list_free(void *l)
{
    atm_list_t * list = (atm_list_t *) l;
    atm_list_entry_t *curr = NULL;
    /* purge list elements */
    while ((curr=atm_list_lpop_entry(list)) != NULL) {
        atm_list_entry_free(curr);
    }
    atm_free(list);
}


void atm_list_push(atm_list_t *list, void *value)
{
    if (list != NULL) {
        atm_list_entry_t * entry = atm_list_entry_new(value);
        entry->list = list;
        /* add the first one */
        if (list->tail == NULL) {
            if (list->head == NULL) {
                list->head = entry;
                list->tail = entry;
            } else {
                atm_log_routine(
                        ATM_LOG_FATAL,"atm_list_push: list currapt");
                exit(ATM_ERROR);
            }
        } else {
            /* normal case */
            list->tail->next = entry;
            entry->prev = list->tail;
            list->tail = entry;
        }
        list->size++;
    }
}


static atm_list_entry_t *atm_list_find_linear(atm_list_t *list, void *hint)
{
    atm_list_entry_t *result = NULL;
    atm_list_entry_t *curr = NULL;         
    if (list != NULL) {
        curr = list->head;       
        while (curr != NULL) {
            if (list->type->match(curr->value, hint)) {
                result = curr;
                break;
            }
            curr = curr->next;    
        }
    }
    return result;
}


void atm_list_del(atm_list_t *list, void *hint)
{
    atm_list_entry_t *curr;
    atm_list_entry_t *prev;
    atm_list_entry_t *next;
    atm_list_entry_t *entry = atm_list_find_linear(list, hint);
    curr = entry;
    if (entry != NULL) {
        prev = curr->prev;
        next = curr->next;
        if (prev == NULL && next == NULL) {
            /* only one entry */
            list->head = NULL; 
            list->tail = NULL;
        } else
        if (prev == NULL && next != NULL) {
            /* head entry */
            next->prev = NULL;
            list->head = next;
        } else
        if (prev != NULL && next == NULL) {
            /* tail entry */
            prev->next = NULL;
            list->tail = prev;
        } else {
            /* normal case */
            prev->next = next;
            next->prev = prev;
        }
        list->size--;
        atm_list_entry_free(curr);
    }
}


void *atm_list_find(atm_list_t *list, void *hint)
{
    void *result = NULL;
    atm_list_entry_t * entry = atm_list_find_linear(list, hint);
    if (entry != NULL) {
        result = entry->value;
    }
    return result;
}


void *atm_list_lpop(atm_list_t *list) {
    void *result = NULL;
    atm_list_entry_t *entry = atm_list_lpop_entry(list);
    if (entry != NULL) {
        result = entry->value;
        atm_free(entry);
    }
    return result;
}


static atm_list_entry_t *atm_list_lpop_entry(atm_list_t *list)
{
    atm_list_entry_t *result = list->head;
    if (result != NULL) {
        list->head = result->next;
        if (list->head != NULL)
            list->head->prev = NULL;
        list->size--;
        atm_list_entry_isol(result);
    }
    return result;
}


void *atm_list_rpop(atm_list_t *list)
{
    void *result = NULL;
    atm_list_entry_t *entry = atm_list_rpop_entry(list);
    if (entry != NULL) {
        result = entry->value;
        atm_free(entry);
    }
    return result;
}


static atm_list_entry_t *atm_list_rpop_entry(atm_list_t *list)
{
    atm_list_entry_t *result = list->tail;
    if (result != NULL) {
        list->tail = result->prev;
        if (list->tail != NULL)
            list->tail->next = NULL;
        list->size--;
        atm_list_entry_isol(result);
    }
    return result;
}


static atm_list_entry_t *atm_list_entry_new (void *value)
{
    atm_list_entry_t *result = NULL;
    result = (atm_list_entry_t *) atm_malloc(sizeof(atm_list_entry_t));
    result->value = value;
    atm_list_entry_isol(result);
    return result;
}


static void atm_list_entry_free(void *e)
{
    atm_list_entry_t *entry = (atm_list_entry_t *) e;
    atm_list_t list = entry->list;
    atm_list_T_t *type = list->type;

    atm_list_entry_isol(entry);
    if (list->deep_free) {
        type->free_value(entry->value);
    }
    atm_free(entry);
}
