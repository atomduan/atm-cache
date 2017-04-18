#ifndef _ATM_LIST_H_INCLUDED_
#define _ATM_LIST_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>


typedef struct atm_list_s       atm_list_t;
typedef struct atm_list_ele_s   atm_list_ele_t;


struct atm_list_s {
    atm_list_ele_t     *head;
    atm_list_ele_t     *tail;
    atm_uint_t          size;
};


struct atm_list_ele_s {
    void               *value;
    atm_list_ele_t     *prev;
    atm_list_ele_t     *next;
};


void atm_list_init();

atm_list_t *atm_list_new();
void atm_list_free(atm_list_t *e);

atm_list_ele_t *atm_list_ele_new (void *value);
void atm_list_ele_free(atm_list_ele_t *e);


#endif /* _ATM_LIST_H_INCLUDED_ */
