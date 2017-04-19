#include <atm_config.h>
#include <atm_core.h>

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
    return result;
}


void atm_list_free(atm_list_t *list)
{
    atm_list_ele_t *curr = NULL;
    /* purge list elements */
    while ((curr=atm_list_lpop(list)) != NULL) {
        atm_list_ele_free(curr);
    }
    atm_free(list);
}


atm_list_ele_t *atm_list_lpop(atm_list_t *list)
{
    atm_list_ele_t *result = list->head;
    if (result != NULL) {
        list->head = result->next;
        if (list->head != NULL)
            list->head->prev = NULL;
        list->size--;
    }
    atm_list_ele_isol(result);
    return result;
}


atm_list_ele_t *atm_list_rpop(atm_list_t *list)
{
    atm_list_ele_t *result = list->tail;
    if (result != NULL) {
        list->tail = result->prev;
        if (list->tail != NULL)
            list->tail->next = NULL;
        list->size--;
    }
    atm_list_ele_isol(result);
    return result;
}


atm_list_ele_t *atm_list_ele_new (void *value)
{
    atm_list_ele_t *result = NULL;
    result = (atm_list_ele_t *) atm_malloc(sizeof(atm_list_ele_t));
    atm_list_ele_isol(result);
    return result;
}


void atm_list_ele_free(atm_list_ele_t *ele)
{
    atm_list_T_t *type = ele->list->type;
    type->free_ele((void *) ele);
    atm_free(ele);
}
