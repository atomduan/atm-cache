#ifndef _ATM_LIST_H_INCLUDED_
#define _ATM_LIST_H_INCLUDED_

#include <atm_core.h>


struct atm_list_s {
    atm_bool_t          deep_free;
    atm_T_t            *type;
    atm_list_entry_t   *head;
    atm_list_entry_t   *tail;
    atm_uint_t          size;
};


struct atm_list_entry_s {
    atm_list_t         *list;
    void               *value;
    atm_list_entry_t   *prev;
    atm_list_entry_t   *next;
};


void atm_list_init();


atm_list_t *atm_list_new(atm_T_t *type);
void atm_list_free(void *list);


void atm_list_push(atm_list_t *list, void *value);
void atm_list_del(atm_list_t *list, void *hint);
void *atm_list_find(atm_list_t *list, void *hint);
void *atm_list_lpop(atm_list_t *list);
void *atm_list_rpop(atm_list_t *list);


#endif /* _ATM_LIST_H_INCLUDED_ */
