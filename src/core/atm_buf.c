#include <atm_core.h>


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


void
atm_buf_flip(atm_buf_t *buf, int len)
{
    atm_block_t *cb = NULL; 

    cb = buf->cb;
    cb->tail = &cb->head[len-1];
    atm_list_push(buf->blocks, cb);

    buf->cb = atm_block_new(ATM_BUF_DEFAULT_LEN);
}


/* public funcs */
atm_int_t
atm_buf_write(atm_buf_t *buf, uint8_t *src, 
        atm_uint_t len)
{
    /* TODO impl */
    return 0;
}


atm_int_t
atm_buf_read(atm_buf_t *buf, uint8_t *dest, 
        atm_uint_t len)
{
    /* TODO impl */
    return 0;
}


atm_str_t *
atm_buf_read_line(atm_buf_t *buf)
{
    atm_str_t *res = NULL;
    return res;
}
