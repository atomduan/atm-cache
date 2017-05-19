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
            /* recycle the readed blk */
            bk = atm_list_lpop(buf->blks);
            atm_blk_new(bk);
            /* peek the new one */
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
    res->aval = 0;
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
            /* account buf bytes */
            atm_atomic_fetch_add(&buf->aval, ret);
            continue;
        }
        result = total>0 ? total:ret;
        break;
    }
    atm_log("atm_buf_read_sock r_buf aval[%lu]", buf->aval);
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
                    /* account buf bytes */
                    int av = -1*ret;
                    atm_atomic_fetch_add(&buf->aval, av);
                    continue;
                }
            }
        }
        result = total>0 ? total:ret;
        break;
    }
    atm_log("atm_buf_write_sock w_buf aval[%lu]", buf->aval);
    return result;
}


/* for session logic calling's funcs */
char *
atm_buf_read_line(atm_buf_t *buf)
{
    char *res = NULL;
    atm_list_iter_t *it = NULL;
    atm_blk_t *bk = NULL;
    atm_uint_t len = 0;
    int find_end = 0;
    
    it = atm_list_iter_new(buf->blks);
    while ((bk=atm_list_next(it)) != NULL) {
        for (atm_uint_t i=bk->ridx; i<bk->widx; ++i) {
            uint8_t b = bk->head[i];
            if (!find_end) {
                if (b == '\r' || b == '\n') {
                    find_end = 1;
                }
                len++;
            } else {
                if (b == '\r' || b == '\n') {
                    len++;
                } else {
                    goto start_build;
                }
            }
        }
    }

start_build:
    atm_list_iter_free(it);
    if (len > 0) {
        res = atm_alloc(len+1); 
        atm_buf_read(buf,res,len);
    }
    return res;
}


atm_int_t
atm_buf_read(atm_buf_t *buf, void *dest, 
        atm_uint_t nbyte)
{
    uint8_t *dptr = (uint8_t *)dest;
    atm_int_t rmn = nbyte;
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
                /* account byte num */
                int av = -1*l;
                atm_atomic_fetch_add(&buf->aval, av);
                if (rmn > 0) continue;
            }
        }
        break;
    }
    atm_log("atm_buf_read r_buf aval[%lu]", buf->aval);
    return total;
}


atm_int_t
atm_buf_write(atm_buf_t *buf, void *src, 
        atm_uint_t nbyte)
{
    uint8_t *sptr = (uint8_t *)src;
    atm_int_t rmn = nbyte;
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
            /* account byte num */
            atm_atomic_fetch_add(&buf->aval, l);
            if (rmn > 0) continue;
        }
        break;
    }
    atm_log("atm_buf_write w_buf aval[%lu]", buf->aval);
    return total;
}
