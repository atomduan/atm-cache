#include <atm_core.h>
/*
 * Private
 * */
static void
atm_blk_reset(atm_blk_t *block);
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
static pthread_mutex_t bp_lock;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_blk_reset(atm_blk_t *block)
{
    if (block != NULL) {
        block->ridx = 0;
        block->widx = 0;
    }
}


static atm_bool_t
atm_blk_pool_release(atm_blk_t *block)
{
    //TODO must thread safe
    atm_bool_t res = ATM_FALSE;
    if (blk_pool == NULL) {
        res = ATM_FALSE;
    } else {
        atm_blk_reset(block);
        pthread_mutex_lock(&bp_lock);
        atm_list_push(blk_pool, block);
        pthread_mutex_unlock(&bp_lock);
    }
    return res;
}


static atm_blk_t *
atm_blk_pool_fetch()
{
    //TODO must thread safe
    atm_blk_t *res = NULL;
    if (blk_pool != NULL) {
        pthread_mutex_lock(&bp_lock);
        res = atm_list_lpop(blk_pool);
        pthread_mutex_unlock(&bp_lock);
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
void
atm_blk_init()
{
    blk_pool = atm_list_new(ATM_BLK_T,
            ATM_FREE_SHALLOW);
    pthread_mutex_init(&bp_lock, NULL);
}


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
