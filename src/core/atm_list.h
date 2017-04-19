#ifndef _ATM_LIST_H_INCLUDED_
#define _ATM_LIST_H_INCLUDED_

#include <atm_core.h>


struct atm_list_s {
    atm_bool_t          shallow_free;
    atm_list_ele_t     *head;
    atm_list_ele_t     *tail;
    atm_uint_t          size;
    atm_list_T_t       *type;
};


struct atm_list_ele_s {
    atm_list_t         *list;
    void               *value;
    atm_list_ele_t     *prev;
    atm_list_ele_t     *next;
};


struct atm_list_T_s {
    atm_bool_t  (* match)(void *ele, void *hint);
    void        (* free_value)(void *v);
};


void atm_list_init();


atm_list_t *atm_list_new(atm_list_T_t *type);
void atm_list_free(void *l);


void atm_list_push(atm_list_t *list, void *value);
void atm_list_del(atm_list_t *list, void *hint);
void *atm_list_find(atm_list_t *list, void *hint);
void *atm_list_lpop(atm_list_t *list);
void *atm_list_rpop(atm_list_t *list);


#endif /* _ATM_LIST_H_INCLUDED_ */
