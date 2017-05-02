#include <atm_core.h>
/*
 * Private
 * */
static atm_T_t ATM_BLK_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
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
atm_blk_new(int size)
{
    atm_blk_t *res = NULL;

    res = atm_alloc(sizeof(atm_blk_t));
    res->size = size;
    res->head = atm_alloc(sizeof(uint8_t)*size);
    res->len = 0;
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
