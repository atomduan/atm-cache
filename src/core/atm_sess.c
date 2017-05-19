#include <atm_core.h>
/*
 * Private
 * */
static void
atm_sess_reply(atm_conn_t *c, char *m);

static void
atm_sess_reset(atm_sess_t *se);

static void
atm_sess_process_cmd(atm_conn_t *c, 
        atm_uint_t argc, atm_str_t **argv);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_sess_reply(atm_conn_t *c, char *m)
{
    atm_str_t *msg = NULL;
    msg = atm_str_new(m);
    msg = atm_str_cat(msg, "\n");
    atm_conn_write_str(c,msg);
    atm_free(msg);
}


static void
atm_sess_reset(atm_sess_t *se)
{
    int i = 0;
    atm_str_t *v = NULL;

    for (;i<se->argc;++i) {
        v = &se->argv[i];
        atm_str_free(v);
    }
    se->argc = 0;
    se->argv = NULL;
}


static void
atm_sess_process_cmd(atm_conn_t *c, 
        atm_uint_t argc, atm_str_t **argv)
{
    atm_str_t *k = NULL;
    atm_str_t *v = NULL;
    atm_str_t *r = NULL;
    atm_str_t *cmd = NULL;

    cmd = argv[0];
    if (atm_str_eqs(cmd, "set")) {
        if (argc == 3) {
            k = argv[1];
            v = argv[2];
            atm_dict_set(atm_ctx->dt, k, v);
            atm_sess_reply(c,"Success");
        } else {
            atm_sess_reply(c,"Invalid argc");
        }
    } else
    if (atm_str_eqs(cmd, "get")) {
        if (argc == 2) {
            k = argv[1];
            r = atm_dict_get(atm_ctx->dt, k);
            atm_conn_write_str(c, atm_str_cat(r,"\n"));
        } else {
            atm_sess_reply(c,"Invalid argc");
        }
    } else {
        atm_sess_reply(c,"Invalid cmd");
    }
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
    return res;
}


void
atm_sess_free(void *sess)
{
    atm_sess_t *se = sess;
    atm_conn_t *conn = se->conn;

    atm_conn_free(conn);
    atm_free(se);
}


void
atm_sess_process(atm_sess_t *se)
{
    atm_str_t   *line = NULL;
    atm_str_t  **argv = NULL;
    atm_str_t  **t = NULL;
    atm_uint_t   argc = 0;
    atm_conn_t * c = NULL;

    c = se->conn;
    line = atm_conn_read_line(c);
    if (line != NULL) {
        atm_log("sess_proc %s", line->val);
        argv = atm_str_split(line->val, line->len); 
        if (argv != NULL) {
            t = argv;
            for (;*t!=NULL; t++) {
                argc++;
            }
            atm_sess_process_cmd(c, argc, argv); 
        }
        atm_sess_reset(se);
    }
}
