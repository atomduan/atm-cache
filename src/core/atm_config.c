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

char *
atm_config_get(char *k)
{
    atm_str_t *key = NULL;
    atm_str_t *value = NULL;
    char *res = NULL;
    
    key = atm_str_new(k);
    value = atm_dict_get(config, key);
    if (value != NULL) {
        res = value->val;
    }

    atm_str_free(key);
    return res;
}

void 
atm_config_set(char *k, char *v)
{
    atm_str_t *key = NULL;
    atm_str_t *value = NULL;
    
    key = atm_str_new(k);
    value = atm_str_new(v);
    atm_dict_set(config, key, value);
}

void 
atm_config_del(char *k)
{
    atm_str_t *key = NULL;
    key = atm_str_new(k);
    atm_dict_del(config, key);
}
