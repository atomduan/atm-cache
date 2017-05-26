#include <atm_core.h>
/*
 * Private
 * */
static void
atm_config_load();


static atm_dict_t *config;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_config_load()
{
    /* load from default config path */
}


/*
 * Public
 * */
void
atm_config_init()
{
    config = atm_dict_new(
            ATM_STR_T,
            ATM_STR_T,
            ATM_FREE_DEEP);
    /* load from conf path */
    atm_config_load();
}


atm_str_t
atm_config_get(atm_str_t k)
{
    atm_str_t res = NULL;
    void *value;

    value = atm_dict_get(config, k);
    if (value != NULL) {
        res = (atm_str_t)value;
    }
    return res;
}


void
atm_config_set(atm_str_t k, atm_str_t v)
{
    atm_dict_set(config, k, v);
}


void
atm_config_del(atm_str_t k)
{
    atm_dict_del(config, k);
}
