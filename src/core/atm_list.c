#include <atm_core.h>


/*
 * Private
 * */


static atm_list_entry_t *
atm_list_entry_new(void *val);

static void 
atm_list_entry_free(void *entry);


/* funcs */
static void 
atm_list_entry_isol(atm_list_entry_t * entry);

static atm_list_entry_t *
atm_list_find_linear(atm_list_t *list, void *entry);

static atm_list_entry_t *
atm_list_lpop_entry(atm_list_t *list);

static atm_list_entry_t *
atm_list_rpop_entry(atm_list_t *list);


/*
 * Public
 * */


atm_T_t ATM_LIST_T = {
    ATM_T_INDEX_LIST,
    atm_list_spec,
    atm_list_match,
    atm_list_hash,
    atm_list_cmp,
    atm_list_str,
    atm_list_free,
};


/* ---------------------IMPLEMENTATIONS--------------------------- */


/*
 * Private
 * */


static atm_list_entry_t *
atm_list_entry_new (void *val)
{
    atm_list_entry_t *res = NULL;

    res = atm_alloc(sizeof(atm_list_entry_t));

    res->val = val;
    atm_list_entry_isol(res);
    return res;
}


static void 
atm_list_entry_free(void *entry)
{
    atm_list_entry_t *e = NULL;
    atm_list_t *list = NULL;
    atm_T_t *v_type = NULL;

    e = (atm_list_entry_t *) entry;
    list = e->list;
    v_type = list->v_type;

    atm_list_entry_isol(e);
    if (list->free_type == ATM_FREE_DEEP) {
        v_type->free(e->val);
    }
    atm_free(e);
}


/* make sure that the entry has no deps with the list position */
static void 
atm_list_entry_isol(atm_list_entry_t * entry)
{
    if (entry != NULL) {
        entry->prev = NULL;
        entry->next = NULL;
    }
}


static atm_list_entry_t *
atm_list_find_linear(atm_list_t *list, void *entry)
{
    atm_list_entry_t *res = NULL;
    atm_list_entry_t *curr = NULL;         

    if (list != NULL) {
        curr = list->head;       
        while (curr != NULL) {
            if (list->v_type->match(curr->val, entry)) {
                res = curr;
                break;
            }
            curr = curr->next;    
        }
    }
    return res;
}


static atm_list_entry_t *
atm_list_lpop_entry(atm_list_t *list)
{
    atm_list_entry_t *res = NULL;

    res = list->head;
    if (res != NULL) {
        list->head = res->next;
        if (list->head != NULL)
            list->head->prev = NULL;
        list->size--;
        atm_list_entry_isol(res);
    }
    return res;
}


static atm_list_entry_t *
atm_list_rpop_entry(atm_list_t *list)
{
    atm_list_entry_t *res = NULL;

    res = list->tail;
    if (res != NULL) {
        list->tail = res->prev;
        if (list->tail != NULL)
            list->tail->next = NULL;
        list->size--;
        atm_list_entry_isol(res);
    }
    return res;
}


/*
 * Public
 * */


void 
atm_list_init()
{
    /*VOID*/
}


atm_list_t *
atm_list_new(atm_T_t *v_type, atm_uint_t f_type)
{
    atm_list_t *res = NULL;

    res = atm_alloc(sizeof(atm_list_t));

    res->head = NULL;
    res->tail = NULL;
    res->size = 0;
    res->v_type = v_type;
    res->free_type = f_type;
    return res;
}


void *
atm_list_spec(void *list)
{
    void *res = NULL;
    return res;
}


atm_bool_t 
atm_list_match(void *list1, void *list2)
{
    atm_bool_t res = ATM_FALSE;
    
    if (list1 == list2) {
        res = ATM_TRUE;
    }
    return res;
}


uint64_t 
atm_list_hash(void *list)
{
    uint64_t res;
    atm_list_t *l; 
    atm_str_t *l_str;

    l = (atm_list_t *) list;
    l_str = atm_str_ptr_str(l); 
    res = atm_hash(l_str->val, l_str->len);

    atm_str_free(l_str);
    return res;
}


atm_int_t 
atm_list_cmp(void *list1, void *list2)
{
    return ATM_CMP_EQ;
}


atm_str_t *
atm_list_str(void *list)
{
    atm_str_t *res;
    atm_list_t *l; 

    l = (atm_list_t *) list;
    if (l != NULL) {
        res = atm_str_fmt(
                "free_type[%d];size[%ld]",
                l->free_type,
                l->size);
    } else {
        res = atm_str_new("NULL");
    }
    return res;
}


void 
atm_list_free(void *list)
{
    atm_list_t * l = (atm_list_t *) list;
    atm_list_entry_t *curr = NULL;
    /* purge list elements */
    while ((curr=atm_list_lpop_entry(l))!=NULL) {
        atm_list_entry_free(curr);
    }
    atm_free(l);
}


void 
atm_list_push(atm_list_t *list, void *val)
{
    atm_list_entry_t * entry;
    if (list != NULL) {
        entry = atm_list_entry_new(val);
        entry->list = list;
        /* add the first one */
        if (list->tail == NULL) {
            if (list->head == NULL) {
                list->head = entry;
                list->tail = entry;
            } else {
                atm_log_rout(ATM_LOG_FATAL,
                    "atm_list_push: list currapt");
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


void 
atm_list_del(atm_list_t *list, void *hint)
{
    atm_list_entry_t *curr = NULL;
    atm_list_entry_t *prev = NULL;
    atm_list_entry_t *next = NULL;
    atm_list_entry_t *entry = NULL;

    entry = atm_list_find(list, hint);
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


void *
atm_list_find(atm_list_t *list, void *hint)
{
    void *res = NULL;
    atm_list_entry_t *entry = NULL;

    entry = atm_list_find_linear(list, hint);
    if (entry != NULL) {
        res = entry->val;
    }
    return res;
}


void *
atm_list_lpop(atm_list_t *list)
{
    void *res = NULL;
    atm_list_entry_t *entry = NULL;

    entry = atm_list_lpop_entry(list);
    if (entry != NULL) {
        res = entry->val;
        atm_free(entry);
    }
    return res;
}


void *
atm_list_rpop(atm_list_t *list)
{
    void *res = NULL;
    atm_list_entry_t *entry = NULL;

    entry = atm_list_rpop_entry(list);
    if (entry != NULL) {
        res = entry->val;
        atm_free(entry);
    }
    return res;
}
