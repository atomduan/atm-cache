#include <atm_core.h>
/*
 * Private
 * */
static atm_bool_t
atm_blk_pool_release(atm_blk_t *block);
static atm_blk_t *
atm_blk_pool_fetch();
static atm_blk_t *
atm_blk_new_raw();
static void
atm_blk_free_raw();

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


static atm_list_t *blk_pool = NULL;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_bool_t
atm_blk_pool_release(atm_blk_t *block)
{
    atm_bool_t res = ATM_FALSE;
    if (blk_pool == NULL) 
        res = ATM_FALSE;
    return res;
}


static atm_blk_t *
atm_blk_pool_fetch()
{
    atm_blk_t *res = NULL;
    if (blk_pool != NULL) {
        //TODO need implements
    }
    return res;
}


static atm_blk_t *
atm_blk_new_raw()
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


static void
atm_blk_free_raw(void *block)
{
    atm_blk_t *b = block;
    if (b != NULL) {
        atm_free(b->head);
        atm_free(b);
    }
}


/*
 * Public
 * */
atm_blk_t *
atm_blk_new()
{
    atm_blk_t *res = NULL;
    res=atm_blk_pool_fetch();
    if (res == NULL) {
        res = atm_blk_new_raw();
    }
    return res;
}


void
atm_blk_free(void *block)
{
    if (block != NULL) {
        atm_blk_t *b = block;
        if (!atm_blk_pool_release(b)) {
            atm_blk_free_raw(block);
        }
    }
}
