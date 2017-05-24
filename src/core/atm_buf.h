#ifndef _ATM_BUF_H_INCLUDED_
#define _ATM_BUF_H_INCLUDED_

#include <atm_core.h>


struct atm_buf_s {
    atm_atomic_t        aval;           
    pthread_mutex_t     _mutex; 
    atm_list_t         *blks;
};


/* public buf type lifecycle */
atm_buf_t *
atm_buf_new();

void
atm_buf_free(void *buf);


/* public funcs */
/* for epoll event call back funcs */
atm_int_t
atm_buf_read_sock(atm_buf_t *buf, 
        atm_socket_t *srcsock);

atm_int_t
atm_buf_write_sock(atm_buf_t *buf, 
        atm_socket_t *destsock);


/* for session logic calling's funcs */
char *
atm_buf_read_line(atm_buf_t *buf);

atm_int_t
atm_buf_read(atm_buf_t *buf, void *dest, 
        atm_uint_t nbyte);

atm_int_t
atm_buf_write(atm_buf_t *buf, void *src, 
        atm_uint_t nbyte);


#endif /* _ATM_BUF_H_INCLUDED_ */
