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
atm_realloc(void *p, atm_uint_t size)
{
    void * ptr = NULL;
    ptr = realloc(p, size);
    if (ptr == NULL) {
        atm_log_rout(ATM_LOG_FATAL, 
            "realloc(%p,%u) failed",p,size);
    }
    return ptr;
}


void 
atm_free(void *ptr) 
{
    free(ptr);
}
