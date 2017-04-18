#include <atm_config.h>
#include <atm_core.h>


void *atm_malloc(atm_uint_t size) 
{
    void * ptr = NULL;
    ptr = malloc(size);
    return ptr;
}


void *atm_calloc(atm_uint_t nmemb, atm_uint_t size)
{
    void * ptr = NULL;
    ptr = calloc(nmemb, size);
    return ptr;
}


void atm_free(void *ptr) 
{
    free(ptr);
}
