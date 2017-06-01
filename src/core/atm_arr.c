#include <atm_core.h>
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
atm_arr_t *
atm_arr_new(atm_uint_t tsize)
{
    atm_arr_t *res = NULL;
    atm_uint_t capacity = ATM_ARR_INITIAL_LEN;
    //atm_log("atm_arr_new tsize is %u", tsize);
    if (tsize > 0) {
        res = atm_alloc(sizeof(atm_arr_t));
        res->_vals = atm_calloc(capacity, tsize);
        res->capacity = capacity;
        res->length = 0;
        res->tsize = tsize;
    }
    return res;
}


void
atm_arr_free(void *arr)
{
    atm_arr_t *a = arr;

    if (a != NULL) {
        atm_free(a->_vals);
        atm_free(a);
    }
}


atm_uint_t
atm_arr_add(atm_arr_t *arr, void *v)
{
    atm_uint_t osz = 0;
    atm_uint_t nsz = 0;
    atm_uint_t add_index = 0;
    int8_t *p = NULL;

    void *old_ptr = NULL;
    void *new_ptr = NULL;
    if (v != NULL) {
        /*a tricky bug here when arr->capacity = 1*/
        atm_uint_t s = (arr->length*3/2) + 1;
        if (s > arr->capacity) {
            old_ptr = arr->_vals;
            osz = arr->capacity*arr->tsize;
            nsz = arr->capacity*arr->tsize*2;
            atm_log("atm_arr grows %u -> %u",osz,nsz);
            new_ptr = atm_realloc(old_ptr,osz,nsz);
            if (new_ptr != old_ptr) {
                arr->capacity *= 2;
                arr->_vals = new_ptr;
            } else {
                atm_log_rout(ATM_LOG_FATAL,
                    "can not realloc atm_arr, "
                    "atm_realloc(%p,%u,%u)",
                    new_ptr,osz,nsz);
                exit(ATM_ERROR);
            }
        }

        p = (int8_t *)arr->_vals;
        p += (arr->tsize * arr->length);
        memcpy(p, v, arr->tsize);
        arr->length += 1;
        add_index = arr->length-1;
    }
    return add_index;
}


void *
atm_arr_get(atm_arr_t *arr, atm_uint_t i)
{
    void *res = NULL;
    if (i < arr->length) {
        int8_t *p = (int8_t *)arr->_vals;
        p += (arr->tsize * i);
        res = (void *)p;
    }
    return res;
}


atm_int_t
atm_arr_set(atm_arr_t *arr,
        atm_uint_t i, void *v)
{
    atm_int_t ret = ATM_ERROR;
    atm_uint_t currlen;
    atm_uint_t tsize;
    uint8_t *d;

    if (arr != NULL) {
        currlen = arr->length;
        tsize = arr->tsize;
        if (i < currlen) {
            d = arr->_vals;
            d += i * tsize;
            memcpy(d,v,tsize);
            ret = ATM_OK;
        }
    }
    return ret;
}


atm_int_t
atm_arr_del(atm_arr_t *arr, atm_uint_t i)
{
    atm_int_t ret = ATM_ERROR;
    atm_uint_t currlen;
    atm_uint_t tsize;
    atm_uint_t currcap;
    atm_uint_t newcap;
    void *vn;
    void *vo;
    uint8_t *d, *s;
    atm_uint_t l = 0;

    if (arr != NULL) {
        currlen = arr->length;
        tsize = arr->tsize;
        currcap = arr->capacity;
        newcap = arr->capacity;
        if (i < currlen) {
            vo = arr->_vals;
            if (currlen*4 < currcap &&
                    ATM_ARR_INITIAL_LEN*2 <= currcap) {
               newcap = arr->capacity / 2;
            }
            vn = atm_calloc(newcap, tsize);
            if (vn != NULL) {
                /* start the head part */
                d = (uint8_t *)vn;
                s = (uint8_t *)arr->_vals;
                l = i * tsize;
                if (l > 0) {
                    memcpy(d, s, l);
                }
                /* start the tail part */
                d += (i) * tsize;
                s += (i+1) * tsize;
                l = ((currlen-1) - i) * tsize;
                if (l > 0) {
                    memcpy(d, s, l);
                }
                arr->_vals = vn;
                arr->length -= 1;
                arr->capacity = newcap;
                atm_free(vo);
                ret = ATM_OK;
            }
        }
    }
    return ret;
}
