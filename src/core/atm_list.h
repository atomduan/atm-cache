#ifndef _ATM_LIST_H_INCLUDED_
#define _ATM_LIST_H_INCLUDED_

#include <atm_core.h>


struct atm_list_s {
    atm_bool_t          deep_free;
    atm_T_t            *v_type;
    atm_list_entry_t   *head;
    atm_list_entry_t   *tail;
    atm_uint_t          size;
};


struct atm_list_entry_s {
    atm_list_t         *list;
    void               *val;
    atm_list_entry_t   *prev;
    atm_list_entry_t   *next;
};


/* public list type lifecycle */
void
atm_list_init();

atm_list_t *
atm_list_new(atm_T_t *v_type);

void *
atm_list_spec(void *list);

atm_bool_t 
atm_list_match(void *list1,void *list2); 

uint64_t 
atm_list_hash(void *list);

atm_int_t 
atm_list_cmp(void *list1, void *list2);

atm_str_t *
atm_list_str(void *list);

void 
atm_list_free(void *list);


/* public funcs */
void 
atm_list_push(atm_list_t *list, void *val);

void 
atm_list_del(atm_list_t *list, void *hint);

void 
*atm_list_find(atm_list_t *list, void *hint);

void 
*atm_list_lpop(atm_list_t *list);

void 
*atm_list_rpop(atm_list_t *list);


#endif /* _ATM_LIST_H_INCLUDED_ */
