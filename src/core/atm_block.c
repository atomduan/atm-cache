#include <atm_core.h>
/*
 * Public
 * */


atm_T_t ATM_BLOCK_T = {
    ATM_T_INDEX_BLOCK,
    atm_block_spec,
    atm_block_match,
    atm_block_hash,
    atm_block_cmp,
    atm_block_str,
    atm_block_free,
};
       

/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */


atm_block_t *
atm_block_new(int size)
{
    atm_block_t *res = NULL;

    res = atm_alloc(sizeof(atm_block_t));
    res->size = size;
    res->head = atm_alloc(size);
    res->tail = res->head;
    return res;
}


void *
atm_block_spec(void *block)
{
    void *res = NULL;
    return res;
}


atm_bool_t 
atm_block_match(void *block1, void *block2)
{
    atm_bool_t res = ATM_FALSE;
    if (block1 == block2) {
        res = ATM_TRUE;
    }
    return res;
} 


uint64_t 
atm_block_hash(void *block)
{
    uint64_t res = 0;
    atm_block_t *b = NULL; 
    atm_str_t *b_str = NULL;

    b = (atm_block_t *) block;
    b_str = atm_str_ptr_str(b); 
    res = atm_hash(b_str->val,b_str->len);

    atm_str_free(b_str);
    return res;
}


atm_int_t 
atm_block_cmp(void *block1, void *block2)
{
    return ATM_CMP_EQ;
}


atm_str_t *
atm_block_str(void *block)
{
    atm_str_t *res = NULL;
    return res;
}


void
atm_block_free(void *block)
{
    atm_block_t *b = block;
    if (b != NULL) {
        atm_free(b->head);
        atm_free(b);
    }
}
