#include <atm_core.h>
/*
 * Private
 * */
static void
atm_sess_reply(atm_sess_t *se, char *m);

static void
atm_sess_reset(atm_sess_t *se);

static atm_str_t *
atm_sess_read_line(atm_sess_t *se);

static void
atm_sess_write(atm_sess_t *se, 
        atm_str_t *s);

static void
atm_sess_process_cmd(atm_sess_t *se, 
        atm_uint_t argc, atm_str_t **argv);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_sess_reply(atm_sess_t *se, char *m)
{
    atm_str_t *msg = NULL;
    msg = atm_str_new(m);
    atm_sess_write(se, atm_str_cat(msg,"\n"));
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


static atm_str_t *
atm_sess_read_line(atm_sess_t *se)
{
    atm_buf_t   *r_buf = NULL;
    r_buf = se->r_buf;
    return atm_buf_read_line(r_buf);
}


static void
atm_sess_write(atm_sess_t *se,
        atm_str_t *s)
{
    atm_buf_t   *w_buf = NULL;

    w_buf = se->w_buf;
    atm_buf_write(w_buf, 
        (uint8_t *) s->val, s->len);

    atm_conn_wnotify(se->conn);
}


static void
atm_sess_process_cmd(atm_sess_t *se, 
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
            atm_sess_reply(se,"Success");
        } else {
            atm_sess_reply(se,"Invalid argc");
        }
    } else
    if (atm_str_eqs(cmd, "get")) {
        if (argc == 2) {
            k = argv[1];
            r = atm_dict_get(atm_ctx->dt, k);
            atm_sess_write(se, 
                    atm_str_cat(r,"\n"));
        } else {
            atm_sess_reply(se,"Invalid argc");
        }
    } else {
        atm_sess_reply(se,"Invalid cmd");
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
    res->r_buf = atm_buf_new();
    res->w_buf = atm_buf_new();
    res->conn = conn;
    return res;
}


void
atm_sess_free(void *sess)
{
    atm_sess_t *se = sess;
    atm_conn_t *conn = se->conn;
    atm_buf_t *rbuf = se->r_buf;
    atm_buf_t *wbuf = se->w_buf;

    atm_conn_free(conn);
    atm_buf_free(rbuf);
    atm_buf_free(wbuf);
    atm_free(se);
}


void
atm_sess_process(atm_sess_t *se)
{
    atm_str_t   *line = NULL;
    atm_str_t  **argv = NULL;
    atm_str_t  **t = NULL;
    atm_uint_t   argc = 0;

    line = atm_sess_read_line(se);
    if (line != NULL) {
        atm_log("sess_proc %s", line->val);
        argv = atm_str_split(line->val, line->len); 
        if (argv != NULL) {
            t = argv;
            for (;*t!=NULL; t++) {
                argc++;
            }
            atm_sess_process_cmd(se, argc, argv); 
        }
        atm_sess_reset(se);
    }
}
