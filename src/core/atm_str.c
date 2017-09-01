#include <atm_core.h>
/*
 * Private
 * */
static char *
atm_str_mtrim(char *s);
static atm_str_t
atm_str_grow(atm_str_t str, atm_uint_t need);
static atm_uint_t
atm_str_size(atm_uint_t slen);
static atm_str_hdr_t *
atm_str_get_hdr(atm_str_t str);
static atm_str_t
atm_str_get_str(atm_str_hdr_t *hdr);


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
static char *
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


static atm_str_t
atm_str_grow(atm_str_t str, atm_uint_t need)
{
    atm_str_t res = NULL;
    atm_str_hdr_t *hdr;
    atm_str_hdr_t *newhdr;
    atm_uint_t hdrlen;
    atm_uint_t avail;
    atm_uint_t newsz;
    atm_uint_t newlen;

    if (need > 0) {
        if (str != NULL) {
            hdr = atm_str_get_hdr(str);
            avail = hdr->cap - hdr->len;
            if (avail < need) {
                hdrlen = sizeof(atm_str_hdr_t);
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


static atm_str_hdr_t *
atm_str_get_hdr(atm_str_t str)
{
    void *res = NULL;
    atm_uint_t hdrlen;

    if (str != NULL) {
        hdrlen = sizeof(atm_str_hdr_t);
        res = ((uint8_t *)str) - hdrlen;
    }
    return (atm_str_hdr_t *)res;
}


static atm_str_t
atm_str_get_str(atm_str_hdr_t *hdr)
{
    void *res = NULL;
    atm_uint_t hdrlen;

    if (hdr != NULL) {
        hdrlen = sizeof(atm_str_hdr_t);
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
    atm_str_hdr_t *hdr;
    atm_str_hdr_t *newhdr;
    atm_uint_t hdrlen;
    atm_uint_t size;

    if (str != NULL) {
        hdr = atm_str_get_hdr(str);
        hdrlen = sizeof(atm_str_hdr_t);
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
    atm_str_hdr_t *ash;
    atm_uint_t hdrlen;
    atm_uint_t size;
    atm_uint_t slen;

    if (str != NULL) {
        slen = strlen(str);
        hdrlen = sizeof(atm_str_hdr_t);
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
    atm_str_hdr_t *hdr;

    s = (atm_str_t) str;
    hdr = atm_str_get_hdr(s);
    atm_free(hdr);
}


/* public funcs */
atm_uint_t
atm_str_len(atm_str_t str)
{
    atm_uint_t res = 0;
    atm_str_hdr_t *h;
    atm_uint_t hdrlen;
    void *tmp;

    if (str != NULL) {
        hdrlen = sizeof(atm_str_hdr_t);
        tmp = ((uint8_t *)str) - hdrlen;
        h = (atm_str_hdr_t *)tmp;
        res = h->len;
    }
    return res;
}


atm_bool_t
atm_str_eq(atm_str_t str1, atm_str_t str2)
{
    atm_bool_t res = ATM_FALSE;
    res = !atm_str_cmp(str1, str2);
    return res;
}


atm_bool_t
atm_str_eqs(atm_str_t str1, char *s)
{
    atm_bool_t res = ATM_FALSE;
    atm_str_t str2;

    str2 = atm_str_new(s);
    res = !atm_str_cmp(str1, str2);
    atm_str_free(str2);
    return res;
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


atm_str_t
atm_str_trim(atm_str_t str, const char *cset)
{
    atm_str_t s = str;
    char *start, *end, *sp, *ep;
    size_t len;

    sp = start = s;
    ep = end = s+atm_str_len(s)-1;
    while (sp<=end && strchr(cset,*sp)) sp++;
    while (ep>sp && strchr(cset,*ep)) ep--;
    len = (sp>ep) ? 0 : ((ep-sp)+1);/*TODO, pointer - pointer is what??*/
    if (s!=sp) memmove(s,sp,len);
    s[len] = '\0';
    atm_str_setlen(s,len);
    return s;
}


atm_str_t
atm_str_ptr(void *p)
{
    return atm_str_fmt("%p",p);
}


atm_str_t
atm_str_cats(atm_str_t dest, char *src)
{
    atm_uint_t slen;
    atm_str_hdr_t *h;

    if (src != NULL) {
        slen = strlen(src);
        dest = atm_str_grow(dest,slen);
        h = atm_str_get_hdr(dest);
        h->len += slen;
        strcat(dest, src);
    }
    return dest;
}


atm_str_t
atm_str_cats_dup(atm_str_t dest, char *src)
{
    atm_str_t dup = NULL;
    atm_uint_t slen;
    atm_str_hdr_t *h;

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
    atm_uint_t slen;
    atm_str_hdr_t *h;

    if (src != NULL) {
        slen = atm_str_len(src);
        dest = atm_str_grow(dest,slen);
        h = atm_str_get_hdr(dest);
        h->len += slen;
        strcat(dest, src);
    }
    return dest;
}


atm_str_t
atm_str_cat_dup(atm_str_t dest, atm_str_t src)
{
    atm_str_t dup = NULL;
    atm_uint_t slen;
    atm_str_hdr_t *h;

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
atm_str_split(char *s, char sep, int len)
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
                if (*p == sep) {
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


atm_int_t
atm_str_split_size(atm_str_t *argv)
{
    atm_int_t size = 0;
    atm_str_t *t = NULL;
    if (argv != NULL) {
        for (t=argv;*t!=NULL;t++) size++;
    }
    return size;
}


void
atm_str_split_free(atm_str_t *argv)
{
    atm_str_t *t = NULL;
    if (argv != NULL) {
        for (t=argv;*t!=NULL;t++) {
            atm_str_free(*t);
        }
        atm_free(argv);
    }
}


void
atm_str_setlen(atm_str_t str, atm_uint_t len)
{
    atm_str_hdr_t *hdr;
    if (str != NULL) {
        hdr = atm_str_get_hdr(str);
        hdr->len = len;
    }
}


void
atm_str_tolower(atm_str_t str)
{
    int offset = 'a'-'A';
    int len;
    int i;
    if (str != NULL) {
        len = atm_str_len(str);
        for (i=0; i<len; i++) {
            if (str[i]>='A' && str[i]<='Z')
                str[i] += offset;
        }
    }
}


void
atm_str_toupper(atm_str_t str)
{
    int offset = 'a'-'A';
    int len;
    int i;
    if (str != NULL) {
        len = atm_str_len(str);
        for (i=0; i<len; i++) {
            if (str[i]>='a' && str[i]<='z')
                str[i] -= offset;
        }
    }
}


atm_bool_t
atm_str_isempty(atm_str_t str)
{
    atm_bool_t res = ATM_TRUE;
    if (str != NULL) {
        if (atm_str_len(str)>0) {
            if (!strcmp(str,"\"\"") || !strcmp(str,"''")) {
                res = ATM_TRUE;
            } else {
                res = ATM_FALSE;
            }
        }
    }
    return res;
}
