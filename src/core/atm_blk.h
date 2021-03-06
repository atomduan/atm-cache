#ifndef _ATM_BLK_H_INCLUDED_
#define _ATM_BLK_H_INCLUDED_

#include <atm_core.h>

#define ATM_BLK_DFT_LEN         65536
#define ATM_BLK_POOL_THRESHOLD  64

extern atm_T_t *ATM_BLK_T;


struct atm_blk_s {
    atm_uint_t      size;
    uint8_t        *head;
    atm_uint_t      ridx;
    atm_uint_t      widx;
};


void
atm_blk_init();


atm_blk_t *
atm_blk_new();

void
atm_blk_free(void *block);


#endif /* _ATM_BLK_H_INCLUDED_ */
