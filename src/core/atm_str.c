#include <atm_core.h>
/*
 * Private
 * */
static atm_T_t ATM_STR_TYPE = {
    NULL,
    atm_str_match,
    atm_str_hash,
    atm_str_cmp,
    atm_str_string,
    atm_str_free,
};


/*
 * Public
 * */
atm_T_t *ATM_STR_T = &ATM_STR_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
/* public string type lifecycle */
atm_str_t *
atm_str_new(char *str)
{
    atm_str_t *res = NULL;
    atm_uint_t len = 0;
    char *copy = NULL;

    len = strlen(str);
    copy = atm_alloc(len+1);
    memcpy(copy, str, len);
    res = atm_str_wrp(copy);
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
    res = atm_dict_hash(ss->val, ss->len);
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
atm_str_wrp(char *str)
{
    atm_str_t *res = NULL;
    if (str != NULL) {
        atm_uint_t len = strlen(str);
        res = atm_alloc(sizeof(atm_str_t));
        res->val = str;
        res->len = len;
    }
    return res;
}


atm_bool_t 
atm_str_eq(atm_str_t *s1, atm_str_t *s2) 
{
    atm_bool_t res = ATM_FALSE;
    res = atm_str_cmp(s1, s2);
    return res==ATM_CMP_EQ?ATM_TRUE:ATM_FALSE;
}


atm_bool_t 
atm_str_eqs(atm_str_t *s1, char *s) 
{
    atm_bool_t res = ATM_FALSE;
    atm_str_t *t = atm_str_new(s);
    res = atm_str_cmp(s1,t);
    atm_free(t);
    return res==ATM_CMP_EQ?ATM_TRUE:ATM_FALSE;
}


atm_str_t *
atm_str_fmt(char *fmt,...)
{
    atm_str_t *res = NULL;
    char *s = NULL;
    char temp[ATM_STR_MAXLEN];
    memset(temp, 0, ATM_STR_MAXLEN);

    va_list args;
    va_start(args, fmt);
    vsnprintf(temp,ATM_STR_MAXLEN,fmt,args);
    s = atm_str_mtrim(temp);
    if (s != NULL)
        res = atm_str_wrp(s);
    va_end(args);
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
atm_str_ptr(void *p)
{
    return atm_str_fmt("%p",p);
}


atm_str_t *
atm_str_cat(atm_str_t *dest, char *src)
{
    atm_str_t *res = NULL;
    char * r = NULL;

    /*
     * TODO : too expansive... 
     * TODO : this impl mey mem leak!!!!
     */
    if (dest != NULL) {
        r = dest->val;
        r = strcat(r, src);
    } else {
        r = src;
    }
    
    res = atm_str_new(r);
    return res;
}


atm_str_t **
atm_str_split(char *s, int len)
{
    atm_str_t **res = NULL;
    char *p = s;

    atm_int_t sli = 0;
    char **sl = atm_alloc(sizeof(char *)*len); 

    char qty = 0; /* quote type: 1->', 2->" */
    int inq = 0; /* in the quote */
    int trb = 0; /* in space trim mode */

    atm_int_t i = 0;
    atm_int_t ti = 0;
    char *t = atm_alloc(len+1); 
    while (i < len) {
        if (*p) {
            if (inq > 0) {
                if (*p == '"') {
                    if (qty == '"') {
                        inq--;
                        if (inq > 0) {
                            qty = '\'';
                        } else {
                            qty = 0;
                        }
                    } else
                    if (qty == '\'') {
                        inq++;     
                        qty = '"';
                    }
                } else
                if (*p == '\'') {
                    if (qty == '"') {
                        inq++;
                        qty = '\'';
                    } else
                    if (qty == '\'') {
                        inq--;     
                        if (inq > 0) {
                            qty = '"';
                        } else {
                            qty = 0;
                        }
                    }
                }
                t[ti++] = *p;
            } else
            if (inq == 0) {
                int skip = 0;
                if (*p == '"' || *p == '\'') {
                    qty = *p;
                    inq++;
                }
                if (isspace(*p)) {
                    if (trb == 0) {
                        /* reload a new t */
                        if (strlen(t)>0) {
                            sl[sli++] = t;       
                            t = atm_alloc(len+1);
                            ti = 0;
                        }
                    }
                    skip = 1; trb = 1;
                } else {
                    trb = 0;
                }
                if (!skip) 
                    t[ti++] = *p;
            }
        }
        p++; i++;
    }

    /* add the final one */
    if (strlen(t)>0) {
        if (sli > 0 && sl[sli-1]!=t) {
            sl[sli++] = t;       
        } else 
        if (sli == 0) {
            sl[sli++] = t;       
        }
    }

    /* the last one is sentinal */
    if (sli > 0) {
        res = atm_alloc(sizeof(atm_str_t *)*(sli+1));
        for (i=0; i<sli; ++i) {
            res[i] = atm_str_new(atm_str_mtrim(sl[i]));
            atm_free(sl[i]);
        }
    } else {
        atm_free(t);
    }

    atm_free(sl);

    return res;
}
