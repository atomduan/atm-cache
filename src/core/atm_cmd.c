#include <atm_core.h>
static atm_obj_t *
atm_cmd_para(atm_str_t *argv, atm_uint_t idx);
static atm_int_t
atm_cmd_func_get(atm_sess_t *se);
static atm_int_t
atm_cmd_func_set(atm_sess_t *se);


static atm_cmd_t atm_commands[] = {
    {"get",atm_cmd_func_get, 2},
    {"set",atm_cmd_func_set,-3},
};


static atm_dict_t *atm_cmd_dict;

/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_obj_t *
atm_cmd_para(atm_str_t *argv, atm_uint_t idx)
{
    return atm_obj_new(ATM_STR_T,atm_str_new(argv[idx]));
}


static atm_int_t
atm_cmd_func_set(atm_sess_t *se)
{
    atm_obj_t *k;
    atm_obj_t *v;
    atm_int_t argc = se->argc;
    atm_str_t *argv = se->argv;
    atm_conn_t *c = se->conn;
    atm_cmd_t *cmd = se->cmd;

    if (cmd->arity < 0 || argc == cmd->arity) {
        k = atm_cmd_para(argv,1);
        v = atm_cmd_para(argv,2);
        atm_log("cmd_func_set k[%s],v[%s]",
                atm_obj_str(k),atm_obj_str(v));

        atm_dict_set(atm_ctx->cache_dict,k,v);
        atm_sess_reply(c,"Success");
    } else {
        atm_sess_reply(c,"Invalid argc");
    }
    return ATM_OK;
}


static atm_int_t
atm_cmd_func_get(atm_sess_t *se)
{
    atm_obj_t *k;
    atm_obj_t *v;
    atm_int_t argc = se->argc;
    atm_str_t *argv = se->argv;
    atm_conn_t *c = se->conn;
    atm_cmd_t *cmd = se->cmd;
    atm_str_t msg;

    if (cmd->arity < 0 || argc == cmd->arity) {
        k = atm_cmd_para(argv,1);
        v = atm_dict_get(atm_ctx->cache_dict,k);
        atm_log("cmd_func_get k[%s],v[%s]",
                atm_obj_str(k),atm_obj_str(v));

        msg = atm_str_cats_dup(atm_obj_str(v),"\n");
        atm_conn_write_str(c,msg);
        atm_str_free(msg);
    } else {
        atm_sess_reply(c,"Invalid argc");
    }
    atm_obj_free(k);
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
