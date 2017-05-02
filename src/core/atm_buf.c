#include <atm_core.h>
/*
 * Private
 * */
static void
atm_buf_recb(atm_buf_t *buf, int len);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_buf_recb(atm_buf_t *buf, int len)
{
    atm_blk_t *cb = NULL; 

    cb = buf->cb;
    cb->len = len;
    atm_list_push(buf->blks, cb);
    buf->cb = atm_blk_new();
}


/*
 * Public
 * */
atm_buf_t *
atm_buf_new()
{
    atm_buf_t *res = NULL;

    res = atm_alloc(sizeof(atm_buf_t));
    res->blks = atm_list_new(
            ATM_BLK_T, 
            ATM_FREE_DEEP);

    res->cb = atm_blk_new();
    return res;
}


void
atm_buf_free(void *buf)
{
    atm_buf_t * b = buf;

    if (b != NULL) {
        atm_list_free(b->blks);
        atm_free(b);
    }
}


/* public funcs */
/* for epoll event call back funcs */
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
atm_buf_readf(atm_buf_t *buf, atm_socket_t *dest, 
        atm_uint_t len)
{
    atm_int_t total = 0;
    atm_blk_t *hb = NULL;

    hb = atm_list_lpop(buf->blks);
    if (hb != NULL) {
        total = write(dest->fd, hb->head, hb->len);
        atm_list_clear(buf->blks);
    }

    return total;
}


/* for session logic calling's funcs */
atm_str_t *
atm_buf_read_line(atm_buf_t *buf)
{
    atm_str_t *res = NULL;
    atm_blk_t *hb = NULL;

    uint8_t tmp[1024*16];
    memset(tmp, 0, 1024*16);

    hb = atm_list_lpop(buf->blks);
    for (atm_uint_t i = 0; i < hb->len; ++i) {
        uint8_t b = hb->head[i];
        if (b != '\r') {
            tmp[i] = b;     
        } else {
            break;
        }
    }
    atm_list_clear(buf->blks);
    res = atm_str_new((char *) tmp);
    return res;
}


atm_int_t
atm_buf_read(atm_buf_t *buf, uint8_t *dest, 
        atm_uint_t len)
{
    atm_int_t total = 0;
    atm_blk_t *hb = NULL;

    hb = atm_list_lpop(buf->blks);
    memcpy(dest, hb->head, hb->len);
    atm_list_lpop(buf->blks);
    atm_list_clear(buf->blks);
    return total;
}


atm_int_t
atm_buf_write(atm_buf_t *buf, uint8_t *src, 
        atm_uint_t len)
{
    atm_int_t total = 0;

    atm_uint_t l = 0;
    uint8_t *b = NULL;

    l = buf->cb->len;
    b = buf->cb->head;
    l = atm_min(l, len);

    memcpy(b, src, l);
    atm_buf_recb(buf, total);
    return total;
}
