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
    if (tsize > 0) {
        atm_arr_t *res = NULL;
        res = atm_alloc(sizeof(atm_arr_t));
        res->_vals = atm_calloc(capacity, tsize);
        res->capacity = capacity;
        res->length = 0;
        res->tsize = tsize;
        pthread_mutex_init(&res->mutex, NULL);
    }
    return res;
}


void
atm_arr_free(void *arr)
{
    atm_arr_t *a = arr;

    if (a != NULL) {
        atm_free(a->_vals);
        pthread_mutex_destroy(&a->mutex);
        atm_free(a);
    }
}


atm_uint_t
atm_arr_add(atm_arr_t *arr, void *v)
{
    atm_uint_t add_index = 0;
    int8_t *p = NULL;
    if (v != NULL) {
        pthread_mutex_lock(&arr->mutex);
        atm_uint_t s = arr->length*3 / 2;
        if (s > arr->capacity) {
            arr->capacity *= 2;
            arr->_vals = atm_realloc(arr->_vals, 
                    (arr->capacity) * arr->tsize);
        }

        p = (int8_t *)arr->_vals;
        p += (arr->tsize * arr->length);
        memcpy(p, v, arr->tsize);
        add_index = arr->length;
        arr->length += 1;
        pthread_mutex_unlock(&arr->mutex);
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


void
atm_arr_del(atm_arr_t *arr, atm_uint_t i)
{
    atm_uint_t currlen;
    atm_uint_t tsize;
    atm_uint_t currcap;
    atm_uint_t newcap;
    void *vn = NULL;
    void *vo = NULL;
    uint8_t *d, *s;
    atm_uint_t l = 0;

    pthread_mutex_lock(&arr->mutex);
    currlen = arr->length;
    tsize = arr->tsize;
    currcap = arr->capacity;
    newcap = arr->capacity;
    if (i < currlen) {
        vo = arr->_vals;
        if (currlen*4 < currcap && 
                ATM_ARR_INITIAL_LEN*2 <= currcap) {
           arr->capacity /= 2; 
           newcap = arr->capacity;
        }
        vn = atm_calloc(newcap, tsize);

        /* start the head part */
        d = (uint8_t *)vn;
        s = (uint8_t *)arr->_vals;
        l = i * tsize;
        if (l > 0) {
            memcpy(d, s, l);
        }
        /* start the tail part */
        d += (l) * tsize;
        s += (l+1) * tsize;
        l = ((currlen-1) - i) * tsize;
        if (l > 0) {
            memcpy(d, s, l);
        }
        arr->_vals = vn;
        atm_arr_free(vo);
        arr->length -= 1; 
    }
    pthread_mutex_unlock(&arr->mutex);
}
