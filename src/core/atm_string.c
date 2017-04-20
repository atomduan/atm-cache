#include <atm_core.h>


/* 
 * FUNCTIONS BIGIN
 * */


void 
atm_str_init()
{
    /* TODO */
}


atm_str_t *
atm_str_new(char *str)
{
    atm_str_t *res = NULL;
    atm_uint_t len = 0;
    char *copy = NULL;

    len = strlen(str);
    copy = atm_malloc(len+1);//TODO include 0?
    memset(copy, 0, len+1);
    //TODO bug here, no string copy
    res = atm_malloc(sizeof(atm_str_t));
    res->val = copy;
    res->len = len;
    return res;
}


void
atm_str_free(void *str) 
{
    atm_str_t *s = NULL;

    s = (atm_str_t *) str;
    atm_free(s->val);
    atm_free(s); 
}


atm_str_t *
atm_str_fmt(char *str,...)
{
    atm_str_t *res = NULL;
    return res;
}


atm_str_t *
atm_str_ptr_tostr()
{
    atm_str_t *res = NULL;
    return res;
}


atm_bool_t 
atm_str_equal(const atm_str_t *s1, const atm_str_t *s2) 
{
    atm_int_t res = 0;
    res = strcmp(s1->val, s2->val);
    return res==0?ATM_TRUE:ATM_FALSE;
}
