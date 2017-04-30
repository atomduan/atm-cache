#include <atm_core.h>


static void
atm_buf_recb(atm_buf_t *buf, int len);


/*
 * Public
 * */


atm_T_t ATM_BUF_T = {
    ATM_T_INDEX_BUF,
    atm_buf_spec,
    atm_buf_match,
    atm_buf_hash,
    atm_buf_cmp,
    atm_buf_str,
    atm_buf_free,
};


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_buf_recb(atm_buf_t *buf, int len)
{
    atm_block_t *cb = NULL; 

    cb = buf->cb;
    cb->len = len;
    atm_list_push(buf->blocks, cb);

    buf->cb = atm_block_new(ATM_BUF_DEFAULT_LEN);
}


/*
 * Public
 * */
atm_buf_t *
atm_buf_new()
{
    atm_buf_t *res = NULL;

    res = atm_alloc(sizeof(atm_buf_t));
    res->blocks = atm_list_new(
            &ATM_BLOCK_T, 
            ATM_FREE_SHALLOW);

    res->cb = atm_block_new(
            ATM_BUF_DEFAULT_LEN);
    
    return res;
}


void *
atm_buf_spec(void *buf)
{
    void *res = NULL;
    return res;
}


atm_bool_t 
atm_buf_match(void *buf1, void *buf2)
{
    atm_bool_t res = ATM_FALSE;
    if (buf1 == buf2) {
        res = ATM_TRUE;
    }
    return res;
} 


uint64_t 
atm_buf_hash(void *buf)
{
    uint64_t res = 0;
    atm_buf_t *b = NULL; 
    atm_str_t *b_str = NULL;

    b = buf;
    b_str = atm_str_ptr_str(b); 
    res = atm_hash(b_str->val,b_str->len);

    atm_str_free(b_str);
    return res;
}


atm_int_t 
atm_buf_cmp(void *buf1, void *buf2)
{
    return ATM_CMP_EQ;
}


atm_str_t *
atm_buf_str(void *buf)
{
    atm_str_t *res = NULL;
    return res;
}


void
atm_buf_free(void *buf)
{
    atm_buf_t * b = buf;

    if (b != NULL) {
        atm_list_free(b->blocks);
        atm_free(b);
    }
}


/* public funcs */
atm_int_t
atm_buf_writef(atm_buf_t *buf, atm_socket_t *src, 
        atm_uint_t len)
{
    atm_socket_t *cs = src;
    atm_int_t total = 0;
    int s = 0;
    int size = 0;

    size = buf->cb->size;
    while (ATM_TRUE) {
        s = read(cs->fd, buf->cb->head, size);
        if (s > 0) {
            total += s;
            atm_buf_recb(buf, s);
            if (s == size) {
                continue; 
            }
        }
        break;
    }
    return total;
}


atm_int_t
atm_buf_write(atm_buf_t *buf, uint8_t *src, 
        atm_uint_t len)
{
    atm_int_t total = 0;
    atm_int_t remain = len;
    uint8_t *b = NULL;
    uint8_t *s = src;

    int l = 0;
    int size = 0;

    size = buf->cb->size;
    while (ATM_TRUE) {
        b = buf->cb->head;
        l = atm_min(remain, size);

        memcpy(b, s, l);
        s += l;
        atm_buf_recb(buf, l);
        total += l;

        remain -= size;
        if (remain <= 0) break;
    }
    return total;
}


atm_int_t
atm_buf_readf(atm_buf_t *buf, atm_socket_t *dest, 
        atm_uint_t len)
{
    atm_int_t total = 0;
    atm_block_t *hb = NULL;

    uint8_t *h = NULL;
    uint8_t *t = NULL;

    int r = len;
    int l = 0;
    int s = 0;

    while (ATM_TRUE) {
        hb = buf->blocks->head->val;
        if (hb == NULL) break;
        h = hb->head;
        l = hb->len;
        s = write(dest->fd, h, atm_min(l,r));
        if (s < l && s > 0) {
            h += s;
            t = atm_alloc(sizeof(uint8_t)*hb->size);
            memcpy(t, h, l-s);
            hb->head = t;
            hb->len = l-s;
            atm_free(h);
        } else 
        if (s == l){
            atm_list_lpop(buf->blocks);
        } else {
            break;
        }
        r -= s;
        total += s;
        if (r <= 0) break;
    }
    return total;
}


atm_int_t
atm_buf_read(atm_buf_t *buf, uint8_t *dest, 
        atm_uint_t len)
{
    atm_int_t total = 0;
    atm_block_t *hb = NULL;

    uint8_t *h = NULL;
    uint8_t *t = NULL;
    uint8_t *d = dest;
    atm_uint_t l = 0;
    atm_uint_t r = len;

    while (ATM_TRUE) {
        hb = buf->blocks->head->val;
        h = hb->head;
        l = hb->len;
        if (l <= r) {
            memcpy(d, h, l);
            d += l;
            atm_list_lpop(buf->blocks);
        } else {
            memcpy(d, h, r);
            h += r;
            t = atm_alloc(sizeof(uint8_t)*hb->size);
            memcpy(t, h, l-r);
            hb->head = t;
            hb->len = l-r;
            atm_free(h);
        }
        r -= l;
        if (r <= 0) break;
    }
    return total;
}


atm_str_t *
atm_buf_read_line(atm_buf_t *buf)
{
    atm_str_t *res = NULL;
    atm_block_t *hb = NULL;
    uint8_t *h = NULL;
    int l = 0;
    int i = 0;
    uint8_t *t = NULL;
    int ti = 0;
    uint8_t tmp[1024*16];
    char *s;

    memset(tmp, 0, 1024*16);
    while (ATM_TRUE) {
        hb = buf->blocks->head->val;
        h = hb->head;
        l = hb->len;
        for (i = 0; i < l; ++i, ++h) {
            uint8_t b = hb->head[i];
            if (b != '\r') {
                tmp[ti++] = b;     
            }
            if (ti >= 1024*16-1) break;
        }
        atm_list_lpop(buf->blocks);
    }
    if (i == l) {
        t = atm_alloc(sizeof(uint8_t)*hb->size);
        memcpy(t, h, l-i);
        hb->head = t;
        hb->len = l-i;
        atm_free(h);
    }
    s = (char *) tmp; 
    res = atm_str_new(s);
    return res;
}
