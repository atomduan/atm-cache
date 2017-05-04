#ifndef _ATM_BUF_H_INCLUDED_
#define _ATM_BUF_H_INCLUDED_

#include <atm_core.h>



struct atm_buf_s {
    atm_blk_t         *cb;
    atm_list_t      *blks;
};


/* public buf type lifecycle */
atm_buf_t *
atm_buf_new();

void
atm_buf_free(void *buf);


/* public funcs */
/* for epoll event call back funcs */
atm_int_t
atm_buf_writef(atm_buf_t *buf, 
        atm_socket_t *srcfd);

atm_int_t
atm_buf_readf(atm_buf_t *buf, 
        atm_socket_t *destfd);


/* for session logic calling's funcs */
atm_str_t *
atm_buf_read_line(atm_buf_t *buf);

atm_int_t
atm_buf_read(atm_buf_t *buf, uint8_t *dest, 
        atm_uint_t len);

atm_int_t
atm_buf_write(atm_buf_t *buf, uint8_t *src, 
        atm_uint_t len);

#endif /* _ATM_BUF_H_INCLUDED_ */
