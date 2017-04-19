#include <atm_core.h>


static void atm_list_ele_isol(atm_list_ele_t * e);
static atm_list_ele_t *atm_list_find_linear(atm_list_t *list, void *key);
static atm_list_ele_t *atm_list_lpop_ele(atm_list_t *list);
static atm_list_ele_t *atm_list_rpop_ele(atm_list_t *list);
static atm_list_ele_t *atm_list_ele_new(void *value);
static void atm_list_ele_free(void *e);


/* make sure that the ele has no deps with the list position */
static void atm_list_ele_isol(atm_list_ele_t * e)
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
    result->shallow_free = ATM_TRUE;
    return result;
}


void atm_list_free(void *l)
{
    atm_list_t * list = (atm_list_t *) l;
    atm_list_ele_t *curr = NULL;
    /* purge list elements */
    while ((curr=atm_list_lpop_ele(list)) != NULL) {
        atm_list_ele_free(curr);
    }
    atm_free(list);
}


void atm_list_push(atm_list_t *list, void *value)
{
    if (list != NULL) {
        atm_list_ele_t * ele = atm_list_ele_new(value);
        ele->list = list;
        /* add the first one */
        if (list->tail == NULL) {
            if (list->head == NULL) {
                list->head = ele;
                list->tail = ele;
            } else {
                atm_log_routine(
                        ATM_LOG_FATAL,"atm_list_push: list currapt");
                exit(ATM_ERROR);
            }
        } else {
            /* normal case */
            list->tail->next = ele;
            ele->prev = list->tail;
            list->tail = ele;
        }
        list->size++;
    }
}


static atm_list_ele_t *atm_list_find_linear(atm_list_t *list, void *hint)
{
    atm_list_ele_t *result = NULL;
    atm_list_ele_t *curr = NULL;         
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
    atm_list_ele_t *curr;
    atm_list_ele_t *prev;
    atm_list_ele_t *next;
    atm_list_ele_t *ele = atm_list_find_linear(list, hint);
    curr = ele;
    if (ele != NULL) {
        prev = curr->prev;
        next = curr->next;
        if (prev == NULL && next == NULL) {
            /* only one ele */
            list->head = NULL; 
            list->tail = NULL;
        } else
        if (prev == NULL && next != NULL) {
            /* head ele */
            next->prev = NULL;
            list->head = next;
        } else
        if (prev != NULL && next == NULL) {
            /* tail ele */
            prev->next = NULL;
            list->tail = prev;
        } else {
            /* normal case */
            prev->next = next;
            next->prev = prev;
        }
        list->size--;
        atm_list_ele_free(curr);
    }
}


void *atm_list_find(atm_list_t *list, void *hint)
{
    void *result = NULL;
    atm_list_ele_t * ele = atm_list_find_linear(list, hint);
    if (ele != NULL) {
        result = ele->value;
    }
    return result;
}


void *atm_list_lpop(atm_list_t *list) {
    void *result = NULL;
    atm_list_ele_t *ele = atm_list_lpop_ele(list);
    if (ele != NULL) {
        result = ele->value;
        atm_free(ele);
    }
    return result;
}


static atm_list_ele_t *atm_list_lpop_ele(atm_list_t *list)
{
    atm_list_ele_t *result = list->head;
    if (result != NULL) {
        list->head = result->next;
        if (list->head != NULL)
            list->head->prev = NULL;
        list->size--;
        atm_list_ele_isol(result);
    }
    return result;
}


void *atm_list_rpop(atm_list_t *list)
{
    void *result = NULL;
    atm_list_ele_t *ele = atm_list_rpop_ele(list);
    if (ele != NULL) {
        result = ele->value;
        atm_free(ele);
    }
    return result;
}


static atm_list_ele_t *atm_list_rpop_ele(atm_list_t *list)
{
    atm_list_ele_t *result = list->tail;
    if (result != NULL) {
        list->tail = result->prev;
        if (list->tail != NULL)
            list->tail->next = NULL;
        list->size--;
        atm_list_ele_isol(result);
    }
    return result;
}


static atm_list_ele_t *atm_list_ele_new (void *value)
{
    atm_list_ele_t *result = NULL;
    result = (atm_list_ele_t *) atm_malloc(sizeof(atm_list_ele_t));
    result->value = value;
    atm_list_ele_isol(result);
    return result;
}


static void atm_list_ele_free(void *e)
{
    atm_list_ele_t *ele = (atm_list_ele_t *) e;
    atm_list_T_t *type = ele->list->type;
    atm_bool_t shallow = ele->list->shallow_free;

    atm_list_ele_isol(ele);
    if (ATM_FALSE == shallow) {
        type->free_value(ele->value);
    }
    atm_free(ele);
}
