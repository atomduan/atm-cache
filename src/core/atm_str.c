#include <atm_core.h>


/*
 * Public
 * */


atm_T_t ATM_STR_T = {
    ATM_T_INDEX_STR,
    atm_str_spec,
    atm_str_match,
    atm_str_hash,
    atm_str_cmp,
    atm_str_string,
    atm_str_free,
};


/* ---------------------IMPLEMENTATIONS--------------------------- */

/* public string type lifecycle */

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
    copy = atm_alloc(len+1);
    memset(copy, 0, len+1);
    memcpy(copy, str, len);
    res = atm_str_wrp(copy, len);
    return res;
}


void *
atm_str_spec(void *str)
{
    void *res = NULL;
    return res;
}


atm_bool_t 
atm_str_match(void *str1, void *str2)
{
    atm_str_t *s1 = (atm_str_t *) str1;
    atm_str_t *s2 = (atm_str_t *) str2;
    return atm_str_eq(s1,s2);
}


uint64_t 
atm_str_hash(void *str)
{
    uint64_t res = 0;
    atm_str_t *ss = NULL;

    ss = (atm_str_t *) str;
    res = atm_hash(ss->val, ss->len);
    return res;
}


atm_int_t 
atm_str_cmp(void *str1, void *str2)
{
    atm_int_t res = ATM_CMP_EQ;
    atm_str_t *s1 = NULL;
    atm_str_t *s2 = NULL;

    s1 = (atm_str_t *) str1;
    s2 = (atm_str_t *) str2;
    if (s1 == s2) {
        res = ATM_CMP_EQ;
    } else {
        res = strcmp(s1->val,s2->val);
    }
    return res;
}


atm_str_t *
atm_str_string(void *str)
{
    return (atm_str_t *) str;
}


void
atm_str_free(void *str) 
{
    atm_str_t *s = NULL;

    s = (atm_str_t *) str;
    atm_free(s->val);
    atm_free(s); 
}


/* public funcs */


atm_str_t *
atm_str_wrp(char *str, atm_uint_t len)
{
    atm_str_t *res = NULL;
    res = atm_alloc(sizeof(atm_str_t));
    res->val = str;
    res->len = len;
    return res;
}


atm_bool_t 
atm_str_eq(atm_str_t *s1, atm_str_t *s2) 
{
    atm_bool_t res = ATM_FALSE;
    res = atm_str_cmp(s1->val,s2->val);
    return res==ATM_CMP_EQ?ATM_TRUE:ATM_FALSE;
}


atm_str_t *
atm_str_fmt(char *fmt,...)
{
    atm_str_t *res = NULL;
    va_list args;
    va_start(args, fmt);
    res = atm_str_vfmt(fmt, args);
    va_end(args);
    return res;
}


atm_str_t *
atm_str_vfmt(char *fmt, va_list args)
{
    atm_str_t *res = NULL;
    atm_uint_t size = 0;
    atm_uint_t len = 0;
    char *temp = NULL;
    char *dest = NULL;

    size = ATM_STR_FMT_MAX_LEN;
    if (fmt != NULL) {
        temp = atm_alloc(size);
        vsnprintf(temp, size, fmt, args);
        dest = atm_str_mtrim(temp);
        if (dest != NULL)
            len = strlen(dest);
            res = atm_str_wrp(dest, len);
    }
    atm_free(temp);
    return res;
}


char *
atm_str_mtrim(char *s)
{
    char *res = NULL;
    atm_int_t len = 0;

    if (s != NULL) {
        len = strlen(s);
        res = atm_alloc(len+1);
        memcpy(res,s,len); 
    }
    return res;
}


atm_str_t *
atm_str_ptr_str(void *p)
{
    return atm_str_fmt("%p",p);
}
