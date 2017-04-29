#ifndef _ATM_BLOCK_H_INCLUDED_
#define _ATM_BLOCK_H_INCLUDED_

#include <atm_core.h>


extern atm_T_t ATM_BLOCK_T;


struct atm_block_s {
    atm_uint_t      size;
    uint8_t        *head;
    uint8_t        *tail;
};


atm_block_t *
atm_block_new(int size);

void *
atm_block_spec(void *block);

atm_bool_t 
atm_block_match(void *block1, void *block2); 

uint64_t 
atm_block_hash(void *block);

atm_int_t 
atm_block_cmp(void *block1, void *block2);

atm_str_t *
atm_block_str(void *block);

void
atm_block_free(void *block);


#endif /* _ATM_BLOCK_H_INCLUDED_ */
