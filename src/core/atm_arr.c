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
        pthread_rwlock_init(&res->rwlk, NULL);
    }
    return res;
}


void
atm_arr_free(void *arr)
{
    atm_arr_t *a = arr;

    if (a != NULL) {
        atm_free(a->_vals);
        pthread_rwlock_destroy(&a->rwlk);
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
        pthread_rwlock_wrlock(&arr->rwlk);
        atm_uint_t s = arr->length*3 / 2;
        if (s > arr->capacity) {
            old_ptr = arr->_vals;
            osz = arr->capacity*arr->tsize;
            nsz = arr->capacity*arr->tsize*2;
            atm_log("atm_arr growns %u -> %u",osz,nsz);
            new_ptr = atm_realloc(old_ptr,osz,nsz);
            if (new_ptr != old_ptr) {
                arr->capacity *= 2;
            } else {
                atm_log_rout(ATM_LOG_FATAL, 
                    "can not realloc atm_arr, "
                    "atm_realloc(%p,%u,%u)",
                    new_ptr,osz,nsz);
            }
        }

        p = (int8_t *)arr->_vals;
        p += (arr->tsize * arr->length);
        memcpy(p, v, arr->tsize);
        add_index = arr->length;
        arr->length += 1;
        pthread_rwlock_unlock(&arr->rwlk);
    }
    return add_index;
}


void *
atm_arr_get(atm_arr_t *arr, atm_uint_t i)
{
    void *res = NULL;
    pthread_rwlock_rdlock(&arr->rwlk);
    if (i < arr->length) {
        int8_t *p = (int8_t *)arr->_vals;
        p += (arr->tsize * i);
        res = (void *)p;
    }
    pthread_rwlock_unlock(&arr->rwlk);
    return res;
}


atm_int_t
atm_arr_del(atm_arr_t *arr, atm_uint_t i)
{
    atm_int_t ret = ATM_ERROR;

    atm_uint_t currlen;
    atm_uint_t tsize;
    atm_uint_t currcap;
    atm_uint_t newcap;
    void *vn = NULL;
    void *vo = NULL;
    uint8_t *d, *s;
    atm_uint_t l = 0;

    pthread_rwlock_wrlock(&arr->rwlk);
    currlen = arr->length;
    tsize = arr->tsize;
    currcap = arr->capacity;
    newcap = arr->capacity;
    if (i < currlen) {
        vo = arr->_vals;
        if (currlen*4 < currcap && 
                ATM_ARR_INITIAL_LEN*2 <= currcap) {
           arr->capacity /= 2; 
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
            d += (l) * tsize;
            s += (l+1) * tsize;
            l = ((currlen-1) - i) * tsize;
            if (l > 0) {
                memcpy(d, s, l);
            }
            arr->_vals = vn;
            arr->length -= 1; 
            arr->capacity = newcap; 
            atm_arr_free(vo);
            ret = ATM_OK;
        }
    }
    pthread_rwlock_unlock(&arr->rwlk);
    return ret;
}
