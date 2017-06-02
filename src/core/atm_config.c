#include <atm_core.h>
/*
 * Private
 * */
static void
atm_config_load();


static atm_config_t config;


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
    atm_config_load();
    atm_ctx->config = &config;
}
