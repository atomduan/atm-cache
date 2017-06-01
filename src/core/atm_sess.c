#include <atm_core.h>
/*
 * Private
 * */
static void
atm_sess_chstat(atm_sess_t *se, atm_sess_state st);
static atm_int_t
atm_sess_reset(atm_sess_t *se);
static atm_int_t
atm_sess_process_cmd(atm_sess_t *se);
static atm_int_t
atm_sess_parsing(atm_sess_t *se);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_sess_chstat(atm_sess_t *se,
        atm_sess_state st)
{
    se->state = st;
    se->state_continue = ATM_TRUE;
}


static atm_int_t
atm_sess_reset(atm_sess_t *se)
{
    atm_str_t *t = NULL;
    if (se != NULL) {
        if (se->argv != NULL) {
            for (t=se->argv;*t!=NULL;t++) {
                atm_str_free(*t);
            }
            atm_free(se->argv);
        }
        se->argc = 0;
        se->argv = NULL;
        se->cmd = NULL;
        se->state = sess_init;
        se->state_continue = ATM_FALSE;
    }
    return ATM_OK;
}


static atm_int_t
atm_sess_process_cmd(atm_sess_t *se)
{
    atm_conn_t *c = se->conn;
    atm_log("sess_process_cmd");
    if (se->cmd != NULL) {
        se->cmd->func(se);
    } else {
        atm_sess_reply(c,"Invalid cmd");
    }
    atm_sess_chstat(se,sess_finish);
    return ATM_OK;
}


static atm_int_t
atm_sess_parsing(atm_sess_t *se)
{
    atm_str_t   line;
    atm_str_t   cmd_name;
    atm_uint_t  argc;
    atm_str_t  *argv;

    argc = 0;
    se->state_continue = ATM_FALSE;
    line = atm_conn_read_line(se->conn);

    if (line != NULL) {
        atm_log("sess_proc %s", line);
        atm_uint_t len = atm_str_len(line);
        argv = atm_str_split(line,len);
        if (argv != NULL) {
            atm_str_t *t = NULL;
            for (t=argv, argc=0; *t!=NULL; t++) {
                if (argc == 0) cmd_name = *t;
                argc++;
            }
            se->argc = argc;
            se->argv = argv;
            se->cmd = atm_cmd_getcmd(cmd_name);
        }
        atm_sess_chstat(se,sess_process_cmd);
        atm_str_free(line);
    }
    return ATM_OK;
}


/*
 * Public
 * */
atm_sess_t *
atm_sess_new(atm_conn_t *conn)
{
    atm_sess_t *res = NULL;

    res = atm_alloc(sizeof(atm_sess_t));
    res->conn = conn;
    res->state = sess_init;
    res->state_continue = ATM_FALSE;
    res->cmd = NULL;
    return res;
}


void
atm_sess_free(void *sess)
{
    atm_sess_t *se = sess;
    atm_conn_t *conn = se->conn;
    atm_str_t *t = NULL;
    if (se != NULL) {
        if (se->argv != NULL) {
            for (t=se->argv;*t!=NULL;t++) {
                atm_str_free(*t);
            }
            atm_free(se->argv);
        }
        atm_conn_free(conn);
        atm_free(se);
    }
}


atm_int_t
atm_sess_process(atm_sess_t *se)
{
    do {
        se->state_continue = ATM_FALSE;
        switch(se->state) {
            case sess_init:
                atm_sess_chstat(se,sess_parsing);
                break;
            case sess_parsing:
                atm_sess_parsing(se);
                break;
            case sess_process_cmd:
                atm_sess_process_cmd(se);
                break;
            case sess_finish:
                atm_sess_reset(se);
                break;
            case sess_error:
                return ATM_ERROR;
            default:
                atm_log_rout(ATM_LOG_FATAL,
                    "sess_process unknow state");
                exit(ATM_ERROR);
        }
    } while(se->state_continue);
    return ATM_OK;
}


void
atm_sess_reply(atm_conn_t *c, char *m)
{
    atm_str_t s;
    atm_str_t msg;

    s = atm_str_new(m);
    msg = atm_str_cats(s,"\n");
    atm_conn_write_str(c,msg);
    atm_str_free(s);
    atm_str_free(msg);
}
