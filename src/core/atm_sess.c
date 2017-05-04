#include <atm_core.h>
/*
 * Private
 * */
static atm_str_t *
atm_sess_read_line(atm_sess_t *se);

void
atm_sess_write(atm_sess_t *se, 
        atm_str_t *s);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_str_t *
atm_sess_read_line(atm_sess_t *se)
{
    atm_buf_t   *r_buf = NULL;
    r_buf = se->r_buf;
    return atm_buf_read_line(r_buf);
}


void
atm_sess_write(atm_sess_t *se,
        atm_str_t *s)
{
    atm_buf_t   *w_buf = NULL;
    atm_conn_t   *conn = NULL;

    w_buf = se->w_buf;
    atm_buf_write(w_buf, 
            (uint8_t *) s->val, s->len);

    conn = se->conn;
    atm_event_add_event(
            conn->event, ATM_EVENT_WRITE);
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
    atm_conn_t *conn = NULL;
    atm_buf_t *rbuf = NULL;
    atm_buf_t *wbuf = NULL;

    atm_conn_free(conn);
    atm_buf_free(rbuf);
    atm_buf_free(wbuf);
    atm_free(sess);
}


void
atm_sess_process(atm_sess_t *se)
{
    atm_str_t   *line = NULL;

    line = atm_sess_read_line(se);
    if (line != NULL) {
        atm_log("sess_proc %s", line->val);
        atm_sess_write(se, atm_str_cat(line,"\n"));
    }
}
