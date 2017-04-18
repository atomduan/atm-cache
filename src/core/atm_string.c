#include <atm_config.h>
#include <atm_core.h>


void atm_string_init()
{
    /* DO Nothing */
}


atm_string_t *atm_string_new(atm_str_t str)
{
    atm_string_t *result = NULL;
    atm_uint_t len = strlen(str);
    atm_str_t copy = atm_malloc(len+1);//TODO include 0?
    memset(copy, 0, len+1);
    result = atm_malloc(sizeof(atm_string_t));
    result->str = copy;
    result->len = len;
    return result;
}


void atm_string_free(atm_string_t *s) 
{
   atm_free(s->str);
   atm_free(s); 
}


atm_bool_t atm_string_equal( 
        const atm_string_t *s1, const atm_string_t *s2) 
{
    return strcmp(s1->str, s2->str)==0 ? ATM_TRUE : ATM_FALSE;
}
