#ifndef _ATM_ARR_H_INCLUDED_
#define _ATM_ARR_H_INCLUDED_

#include <atm_core.h>


#define ATM_ARR_INITIAL_LEN 32


struct atm_arr_s {
    void       *arr;
    atm_uint_t  length;
    atm_uint_t  size;
    atm_uint_t  step;
};


atm_arr_t *
atm_arr_new(atm_uint_t step);

void
atm_arr_free(void *arr);

atm_uint_t
atm_arr_add(void *e);

void *
atm_arr_get(atm_uint_t i);

void
atm_arr_del(atm_uint_t i);

#endif /* _ATM_ARR_H_INCLUDED_ */
