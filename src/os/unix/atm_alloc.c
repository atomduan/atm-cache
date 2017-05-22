#include <atm_core.h>
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
void *
atm_alloc(atm_uint_t size) 
{
    void * ptr = NULL;
    ptr = malloc(size);
    if (ptr != NULL) {
        memset(ptr, ATM_MEM_ZERO, size);
    } else {
        atm_log_rout(ATM_LOG_FATAL, 
            "malloc(%u) failed",size);
    }
    return ptr;
}


void *
atm_calloc(atm_uint_t nmemb, atm_uint_t size)
{
    void * ptr = NULL;
    ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        atm_log_rout(ATM_LOG_FATAL, 
            "calloc(%u,%u) failed",nmemb,size);
    }
    return ptr;
}


void *
atm_realloc(void *p, atm_uint_t osz, atm_uint_t nsz)
{
    void * ptr = NULL;
    atm_uint_t len = atm_min(osz, nsz);

    ptr = atm_alloc(nsz);
    if (ptr != NULL) {
        if (len > 0) {
            memcpy(ptr, p, len);
        }
        atm_free(p);
    } else {
        atm_log_rout(ATM_LOG_FATAL, 
            "atm_alloc(%p,%u) failed",p,nsz);
        ptr = p;
    }
    return ptr;
}


void 
atm_free(void *ptr) 
{
    free(ptr);
}
