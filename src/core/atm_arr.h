#ifndef _ATM_ARR_H_INCLUDED_
#define _ATM_ARR_H_INCLUDED_

#include <atm_core.h>

#define ATM_ARR_INITIAL_LEN 32


struct atm_arr_s {
    void               *_vals;
    atm_uint_t          capacity;
    atm_uint_t          length;
    atm_uint_t          tsize;
};


atm_arr_t *
atm_arr_new(atm_uint_t tsize);

void
atm_arr_free(void *arr);

atm_uint_t
atm_arr_add(atm_arr_t *arr, void *v);

void *
atm_arr_get(atm_arr_t *arr, atm_uint_t i);

atm_int_t
atm_arr_set(atm_arr_t *arr, atm_uint_t i, void *v);

atm_int_t
atm_arr_del(atm_arr_t *arr, atm_uint_t i);

#endif /* _ATM_ARR_H_INCLUDED_ */
