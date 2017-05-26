#include <atm_core.h>
/*
 * Private
 * */
static atm_T_t ATM_BLK_TYPE = {
    NULL,/* sepc */
    NULL,/* match */
    NULL,/* hash */
    NULL,/* compare */
    NULL,/* str */
    atm_blk_free,
};


/*
 * Public
 * */
atm_T_t *ATM_BLK_T = &ATM_BLK_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
atm_blk_t *
atm_blk_new()
{
    atm_blk_t *res = NULL;
    int size = ATM_BLK_DFT_LEN;

    res = atm_alloc(sizeof(atm_blk_t));
    res->size = size;
    res->head = atm_alloc(sizeof(uint8_t)*size);
    res->ridx = 0;
    res->widx = 0;
    return res;
}


void
atm_blk_free(void *block)
{
    atm_blk_t *b = block;
    if (b != NULL) {
        atm_free(b->head);
        atm_free(b);
    }
}
