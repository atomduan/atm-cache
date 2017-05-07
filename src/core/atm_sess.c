#include <atm_core.h>
/*
 * Private
 * */
static void
atm_sess_reset(atm_sess_t *se);

static atm_str_t *
atm_sess_read_line(atm_sess_t *se);

static void
atm_sess_write(atm_sess_t *se, 
        atm_str_t *s);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
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
    atm_str_t   *r = NULL;

    line = atm_sess_read_line(se);
    if (line != NULL) {
        atm_log("sess_proc %s", line->val);
        atm_dict_set(atm_ctx->dt, atm_str_new("foo"), line);
        r = atm_dict_get(atm_ctx->dt, atm_str_new("foo"));
        //r = line;
        atm_sess_write(se, atm_str_cat(r,"\n"));
        atm_sess_reset(se);
    }
}
