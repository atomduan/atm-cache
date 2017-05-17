#ifndef _ATM_LIST_H_INCLUDED_
#define _ATM_LIST_H_INCLUDED_

#include <atm_core.h>


/* this is a inner type 
 * so we do not define it in atm_types.h 
 * plz do not use it in other file, consciously
 * the global type should be define is atm_types.h Specific Tyes sect
 * */
typedef struct atm_list_entry_s atm_list_entry_t;


struct atm_list_s {
    atm_uint_t          free_type;
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


struct atm_list_iter_s {
    atm_list_t         *list;
    atm_list_entry_t   *curr;
};


/* public list type lifecycle */
atm_list_t *
atm_list_new(atm_T_t *v_type, atm_uint_t f_type);

atm_str_t *
atm_list_str(void *list);

void 
atm_list_free(void *list);


/* public funcs */
void 
atm_list_push(atm_list_t *list, void *val);

void 
atm_list_del(atm_list_t *list, void *hint);

void *
atm_list_find(atm_list_t *list, void *hint);

void *
atm_list_lpop(atm_list_t *list);

void *
atm_list_rpop(atm_list_t *list);

void *
atm_list_lpeek(atm_list_t *list);

void *
atm_list_rpeek(atm_list_t *list);

void
atm_list_clear(atm_list_t *list);

void *
atm_list_round(atm_list_t *list);

atm_list_iter_t *
atm_list_iter_new(atm_list_t *list);

void
atm_list_iter_free(void *iter);

void *
atm_list_next(atm_list_iter_t *iter);

void
atm_list_iter_reset(atm_list_iter_t *iter);


#endif /* _ATM_LIST_H_INCLUDED_ */
