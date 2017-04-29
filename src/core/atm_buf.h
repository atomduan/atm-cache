#ifndef _ATM_BUF_H_INCLUDED_
#define _ATM_BUF_H_INCLUDED_

#include <atm_core.h>

#define ATM_BUF_DEFAULT_LEN     1024


extern atm_T_t ATM_BUF_T;


struct atm_buf_s {
    atm_block_t         *cb;
    atm_list_t      *blocks;
};


/* public buf type lifecycle */
atm_buf_t *
atm_buf_new();

void *
atm_buf_spec(void *buf);

atm_bool_t 
atm_buf_match(void *buf1, void *buf2); 

uint64_t 
atm_buf_hash(void *buf);

atm_int_t 
atm_buf_cmp(void *buf1, void *buf2);

atm_str_t *
atm_buf_str(void *buf);

void
atm_buf_free(void *buf);


/* public funcs */
void
atm_buf_flip(atm_buf_t *buf, int len);

atm_int_t
atm_buf_write(atm_buf_t *buf, uint8_t *src, 
        atm_uint_t len);

atm_int_t
atm_buf_read(atm_buf_t *buf, uint8_t *dest, 
        atm_uint_t len);

atm_str_t *
atm_buf_read_line(atm_buf_t *buf);

#endif /* _ATM_BUF_H_INCLUDED_ */
