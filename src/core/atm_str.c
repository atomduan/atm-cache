#include <atm_core.h>
/*
 * Private
 * */
static atm_str_t
atm_str_grow(atm_str_t str, atm_uint_t need);
static atm_uint_t
atm_str_size(atm_uint_t slen);
static atm_str_hdr *
atm_str_get_hdr(atm_str_t str);
static atm_str_t
atm_str_get_str(atm_str_hdr *hdr);


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
 * Private
 * */
static atm_str_t
atm_str_grow(atm_str_t str, atm_uint_t need)
{
    atm_str_t res = NULL;
    atm_str_hdr *hdr;
    atm_str_hdr *newhdr;
    atm_uint_t hdrlen;
    atm_uint_t avail;
    atm_uint_t newsz;
    atm_uint_t newlen;

    if (need > 0) {
        if (str != NULL) {
            hdr = atm_str_get_hdr(str);
            avail = hdr->cap - hdr->len;
            if (avail < need) {
                hdrlen = sizeof(atm_str_hdr);
                newlen = hdr->len + need;
                newsz = atm_str_size(hdrlen+newlen+1);
                newhdr = atm_alloc(newsz);
                newhdr->cap = newsz-hdrlen-1;
                newhdr->len = newlen;
                res = atm_str_get_str(newhdr);
                memcpy(res, str, hdr->len);
                atm_str_free(str);
            } else {
                res = str;
            }
        } else {
            res = atm_str_new("");
            res = atm_str_grow(res, need);
        }
    } else {
        res = str;
    }
    return res;
}


static atm_uint_t
atm_str_size(atm_uint_t slen)
{
    atm_uint_t res = 0;
    if (slen > 0) {
        atm_uint_t r = slen % ATM_WORDLEN;
        res = (slen - r) + ATM_WORDLEN;
    }
    return res;
}


static atm_str_hdr *
atm_str_get_hdr(atm_str_t str)
{
    void *res = NULL;
    atm_uint_t hdrlen;

    if (str != NULL) {
        hdrlen = sizeof(atm_str_hdr);
        res = ((uint8_t *)str) - hdrlen;
    }
    return (atm_str_hdr *)res;
}


static atm_str_t
atm_str_get_str(atm_str_hdr *hdr)
{
    void *res = NULL;
    atm_uint_t hdrlen;

    if (hdr != NULL) {
        hdrlen = sizeof(atm_str_hdr);
        res = ((uint8_t *)hdr) + hdrlen;
    }
    return (atm_str_t)res;
}


/*
 * Public
 * */
atm_str_t
atm_str_dup(atm_str_t str)
{
    atm_str_t res = NULL;
    atm_str_hdr *hdr;
    atm_str_hdr *newhdr;
    atm_uint_t hdrlen;
    atm_uint_t size;

    if (str != NULL) {
        hdr = atm_str_get_hdr(str);
        hdrlen = sizeof(atm_str_hdr);
        size = hdrlen+hdr->cap+1;
        newhdr = atm_alloc(size);
        memcpy(newhdr, hdr, size);
        res = atm_str_get_str(newhdr);
    }
    return res;
}


atm_str_t
atm_str_new(char *str)
{
    atm_str_t res = NULL;
    atm_str_hdr *ash;
    atm_uint_t hdrlen;
    atm_uint_t size;
    atm_uint_t slen;

    if (str != NULL) {
        slen = strlen(str);
        hdrlen = sizeof(atm_str_hdr);
        size = atm_str_size(hdrlen+slen+1);
        ash = atm_alloc(size);
        ash->cap = size-hdrlen-1;
        ash->len = slen;
        res = atm_str_get_str(ash);
        if (slen > 0)
            memcpy(res, str, slen);
    }
    return res;
}


atm_bool_t
atm_str_match(void *str1, void *str2)
{
    atm_str_t s1 = (atm_str_t) str1;
    atm_str_t s2 = (atm_str_t) str2;
    return atm_str_eq(s1,s2);
}


uint64_t
atm_str_hash(void *str)
{
    uint64_t res = 0;
    atm_str_t ss;
    atm_uint_t len;

    ss = (atm_str_t) str;
    len = atm_str_len(ss);
    res = atm_dict_hash(ss, len);
    return res;
}


atm_int_t
atm_str_cmp(void *str1, void *str2)
{
    atm_str_t s1 = (atm_str_t) str1;
    atm_str_t s2 = (atm_str_t) str2;

    if (s1 == s2)
        return ATM_CMP_EQ;
    if (atm_str_len(s1) > atm_str_len(s2))
        return ATM_CMP_GT;
    if (atm_str_len(s1) < atm_str_len(s2))
        return ATM_CMP_LT;
    return strcmp(s1,s2);
}


atm_str_t
atm_str_string(void *str)
{
    return (atm_str_t) str;
}


void
atm_str_free(void *str)
{
    atm_str_t s;
    atm_str_hdr *hdr;

    s = (atm_str_t) str;
    hdr = atm_str_get_hdr(s);
    atm_free(hdr);
}


/* public funcs */
atm_uint_t
atm_str_len(atm_str_t str)
{
    atm_uint_t res = 0;
    atm_str_hdr *h;
    atm_uint_t hdrlen;
    void *tmp;

    if (str != NULL) {
        hdrlen = sizeof(atm_str_hdr);
        tmp = ((uint8_t *)str) - hdrlen;
        h = (atm_str_hdr *)tmp;
        res = h->len;
    }
    return res;
}


atm_bool_t
atm_str_eq(atm_str_t str1, atm_str_t str2)
{
    atm_bool_t res = ATM_FALSE;
    res = atm_str_cmp(str1, str2);
    return res==ATM_CMP_EQ?ATM_TRUE:ATM_FALSE;
}


atm_bool_t
atm_str_eqs(atm_str_t str1, char *s)
{
    atm_bool_t res = ATM_FALSE;
    atm_str_t str2;

    str2 = atm_str_new(s);
    res = atm_str_cmp(str1, str2);
    atm_str_free(str2);
    return res==ATM_CMP_EQ?ATM_TRUE:ATM_FALSE;
}


atm_str_t
atm_str_fmt(char *fmt,...)
{
    atm_str_t res = NULL;
    char *s;
    char temp[ATM_STR_MAXLEN];
    memset(temp,ATM_MEM_ZERO,ATM_STR_MAXLEN);

    va_list args;
    va_start(args, fmt);
    vsnprintf(temp,ATM_STR_MAXLEN,fmt,args);
    s = atm_str_mtrim(temp);
    if (s != NULL)
        res = atm_str_new(s);
    va_end(args);
    return res;
}


char *
atm_str_mtrim(char *s)
{
    char *res = NULL;
    atm_int_t len;

    if (s != NULL) {
        len = strlen(s);
        res = atm_alloc(len+1);
        memcpy(res,s,len);
    }
    return res;
}


atm_str_t
atm_str_ptr(void *p)
{
    return atm_str_fmt("%p",p);
}


atm_str_t
atm_str_cats(atm_str_t dest, char *src)
{
    atm_str_t dup = NULL;
    atm_uint_t slen;
    atm_str_hdr *h;

    dup = atm_str_dup(dest);
    if (src != NULL) {
        slen = strlen(src);
        dup = atm_str_grow(dup,slen);
        h = atm_str_get_hdr(dup);
        h->len += slen;
        strcat(dup, src);
    }
    return dup;
}


atm_str_t
atm_str_cat(atm_str_t dest, atm_str_t src)
{
    atm_str_t dup = NULL;
    atm_uint_t slen;
    atm_str_hdr *h;

    dup = atm_str_dup(dest);
    if (src != NULL) {
        slen = atm_str_len(src);
        dup = atm_str_grow(dup,slen);
        h = atm_str_get_hdr(dup);
        h->len += slen;
        strcat(dup, src);
    }
    return dup;
}


atm_str_t *
atm_str_split(char *s, int len)
{
    atm_str_t *res = NULL;
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
        res = atm_alloc(sizeof(atm_str_t)*(sli+1));
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
