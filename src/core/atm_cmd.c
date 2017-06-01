#include <atm_core.h>
static atm_int_t
atm_cmd_func_get(atm_sess_t *se);
static atm_int_t
atm_cmd_func_set(atm_sess_t *se);


static atm_cmd_t atm_commands[] = {
    {"get",atm_cmd_func_get},
    {"set",atm_cmd_func_set},
};


static atm_dict_t *atm_cmd_dict;

/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_int_t
atm_cmd_func_set(atm_sess_t *se)
{
    atm_str_t k;
    atm_str_t v;
    atm_uint_t argc = se->argc;
    atm_str_t *argv = se->argv;
    atm_conn_t *c = se->conn;

    if (argc == 3) {
        k = atm_str_new(argv[1]);
        v = atm_str_new(argv[2]);
        atm_dict_set(atm_ctx->dt, k, v);
        atm_sess_reply(c,"Success");
    } else {
        atm_sess_reply(c,"Invalid argc");
    }
    return ATM_OK;
}


static atm_int_t
atm_cmd_func_get(atm_sess_t *se)
{
    atm_str_t k;
    atm_str_t v;
    atm_uint_t argc = se->argc;
    atm_str_t *argv = se->argv;
    atm_conn_t *c = se->conn;
    atm_str_t msg;

    if (argc == 2) {
        k = argv[1];
        v = atm_dict_get(atm_ctx->dt, k);
        msg = atm_str_cats(v,"\n");
        atm_conn_write_str(c,msg);
        atm_str_free(msg);
    } else {
        atm_sess_reply(c,"Invalid argc");
    }
    return ATM_OK;
}


/*
 * Public
 * */
void
atm_cmd_init()
{
    atm_int_t num;
    atm_int_t i;
    atm_cmd_t *cmd;
    atm_str_t cmd_name;

    atm_cmd_dict = atm_dict_new(
            ATM_STR_T,
            NULL,
            ATM_FREE_SHALLOW);
    num = sizeof(atm_commands)/sizeof(atm_cmd_t);
    for (i=0; i<num; i++) {
        cmd = &atm_commands[i];
        cmd_name = atm_str_new(cmd->name);
        atm_dict_set(atm_cmd_dict,cmd_name,cmd);
    }
}

atm_cmd_t *
atm_cmd_getcmd(atm_str_t name)
{
    atm_cmd_t *res = NULL;
    if (atm_cmd_dict != NULL) {
        res = atm_dict_get(atm_cmd_dict,name); 
        if (res != NULL)
            atm_log("####cmd hit........");
    }
    return res;
}
