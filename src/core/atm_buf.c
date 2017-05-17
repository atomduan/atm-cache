#include <atm_core.h>
/*
 * Private
 * */
static atm_blk_t *
atm_buf_get_rblk(atm_buf_t *buf);

static atm_blk_t *
atm_buf_get_wblk(atm_buf_t *buf);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_blk_t *
atm_buf_get_rblk(atm_buf_t *buf)
{
    atm_blk_t *res = NULL;
    atm_blk_t *bk = NULL;

    pthread_mutex_lock(&buf->mutex);
    bk = atm_list_lpeek(buf->blks);
    if (bk != NULL) {
        /* refresh rblk */
        if (bk->ridx == bk->widx 
                && bk->widx == bk->size) {
            atm_list_lpop(buf->blks);
            bk = atm_list_lpeek(buf->blks);
        }
        /* check wether have new data to read */
        if (bk->ridx < bk->widx) {
            res = bk;
        }
    }
    pthread_mutex_unlock(&buf->mutex);
    return bk;
}


static atm_blk_t *
atm_buf_get_wblk(atm_buf_t *buf)
{
    atm_blk_t *bk = NULL;

    pthread_mutex_lock(&buf->mutex);
    bk = atm_list_rpeek(buf->blks);
    if (bk == NULL || bk->size == bk->widx) {
        bk = atm_blk_new(); 
        atm_list_push(buf->blks,bk);
        bk = atm_list_rpeek(buf->blks);
    }
    pthread_mutex_unlock(&buf->mutex);
    return bk;
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
    pthread_mutex_init(&res->mutex, NULL);
    return res;
}


void
atm_buf_free(void *buf)
{
    atm_buf_t * b = buf;

    if (b != NULL) {
        atm_list_free(b->blks);
        pthread_mutex_destroy(&b->mutex);
        atm_free(b);
    }
}


/* public funcs */
/* for epoll event call back funcs */
atm_int_t
atm_buf_read_sock(atm_buf_t *buf, 
        atm_socket_t *src)
{
    atm_socket_t *srcsock = src;
    atm_int_t result = 0;
    atm_int_t total = 0;
    int ret = 0;
    atm_blk_t *bk = NULL;

    while (ATM_TRUE) {
        bk = atm_buf_get_wblk(buf);
        int wi = bk->widx;
        int len = bk->size - bk->widx;
        ret = read(srcsock->fd, &bk->head[wi], len);
        if (ret > 0) {
            bk->widx += ret;
            total += ret;
            continue;
        }
        result = total>0 ? total:ret;
        break;
    }
    return result;
}


atm_int_t
atm_buf_write_sock(atm_buf_t *buf, 
        atm_socket_t *dest)
{
    atm_socket_t *destsock = dest;
    atm_int_t result = 0;
    atm_int_t total = 0;
    int ret = 0;
    atm_blk_t *bk = NULL;

    while(ATM_TRUE) {
        bk = atm_buf_get_rblk(buf);
        if (bk != NULL) {
            int ri = bk->ridx;
            int len = bk->widx - bk->ridx;
            if (len > 0) {
                ret = write(destsock->fd, &bk->head[ri], len);
                if (ret > 0) {
                    bk->ridx += ret;
                    total += ret;
                    continue;
                }
            }
        }
        result = total>0 ? total:ret;
        break;
    }
    return result;
}


/* for session logic calling's funcs */
atm_str_t *
atm_buf_read_line(atm_buf_t *buf)
{
    atm_str_t *res = NULL;
    atm_list_iter_t *it = NULL;
    atm_blk_t *bk = NULL;
    atm_uint_t len = 0;
    uint8_t *tmp = NULL;
    
    it = atm_list_iter_new(buf->blks);
    while ((bk=atm_list_next(it)) != NULL) {
        for (atm_uint_t i=bk->ridx; i<bk->widx; ++i) {
            uint8_t b = bk->head[i];
            if (b != '\r' && b != '\n') {
                len++; 
            } else {
                goto start_build;
            }
        }
    }

start_build:
    atm_list_iter_free(it);
    tmp = atm_alloc(len); 
    atm_buf_read(buf, tmp, len);
    res = atm_str_new((char *)tmp);
    atm_free(tmp);
    return res;
}


atm_int_t
atm_buf_read(atm_buf_t *buf, uint8_t *dest, 
        atm_uint_t len)
{
    uint8_t *dptr = dest;
    atm_int_t rmn = len;
    atm_int_t total = 0;
    atm_blk_t *bk = NULL;

    while(ATM_TRUE) {
        bk = atm_buf_get_rblk(buf);
        if (bk != NULL) {
            int ri = bk->ridx;
            int avl = bk->widx - bk->ridx;
            if (avl > 0) {
                int l = atm_min(avl, rmn);
                memcpy(dptr, &bk->head[ri], l);
                dptr += l;
                bk->ridx += l;
                total += l;
                rmn -= l;
                if (rmn > 0) continue;
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
    uint8_t *sptr = src;
    atm_int_t rmn = len;
    atm_int_t total = 0;
    atm_blk_t *bk = NULL;

    while(ATM_TRUE) {
        bk = atm_buf_get_wblk(buf);
        int wi = bk->widx;
        int avl = bk->size - bk->widx;
        if (avl > 0) {
            int l = atm_min(avl, rmn);
            memcpy(&bk->head[wi],sptr,l);
            sptr += l;
            bk->widx += l;
            total += l;
            rmn -= l;
            if (rmn > 0) continue;
        }
        break;
    }
    return total;
}
