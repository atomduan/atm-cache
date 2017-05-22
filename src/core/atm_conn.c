#include <atm_core.h>
/*
 * Private
 * */
static void
atm_conn_free_notify();

static atm_int_t
atm_conn_free_func(atm_task_t *t);

static void
atm_conn_free_enqueue(atm_conn_t *c);

static void
atm_conn_inactive(atm_conn_t *c);

static void
atm_conn_inter_write(atm_conn_t *c, atm_uint_t wreqs, int wrmn);

static atm_bool_t
atm_conn_yield_write(atm_conn_t *c, atm_uint_t wreqs);

static void
atm_conn_write_notify(atm_conn_t *c);

static atm_int_t 
atm_conn_task_read_raw(atm_conn_t *conn); 

static atm_int_t 
atm_conn_task_read(atm_task_t *t); 

static atm_int_t
atm_conn_task_write_raw(atm_conn_t *conn);

static atm_int_t
atm_conn_task_write(atm_task_t *t); 

static atm_conn_t *
atm_conn_new(atm_socket_t *so);

static atm_conn_listen_t *
atm_conn_listen_new(atm_socket_t *so);

static int
atm_conn_listen_tcp();

static void
atm_conn_handle_accept(atm_event_t *listen_event);

static void
atm_conn_handle_read(atm_event_t *conn_event);

static void
atm_conn_handle_write(atm_event_t *conn_event);


/* task type define for container */
static atm_list_t  *conn_free_list;
static pthread_mutex_t  conn_free_lock;
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_conn_free_notify()
{
    atm_task_t *task = NULL;

    task = atm_task_new(NULL, 
        atm_conn_free_func);
    atm_task_dispatch(task);
}


static atm_int_t
atm_conn_free_func(atm_task_t *t)
{
    atm_conn_t *c = NULL; 
    atm_conn_t *unproc = NULL;
    atm_sess_t *sess = NULL;

    pthread_mutex_lock(&conn_free_lock);
    while ((c = atm_list_lpop(conn_free_list)) != NULL) {
        /* all conn has been process */
        if (c == unproc) {
            if (!c->on_write && !c->on_read) {
                sess = c->session; 
                atm_sess_free(sess);
            } else {
                atm_list_push(conn_free_list, c);
            }
            break;
        }
        /* do normal conn check */
        if (!c->on_write && !c->on_read) {
            sess = c->session; 
            atm_sess_free(sess);
        } else {
            if (unproc == NULL) {
                unproc = c;
            }
            atm_list_push(conn_free_list, c);
        }
    }
    pthread_mutex_unlock(&conn_free_lock);
    return ATM_OK;
}


static void
atm_conn_free_enqueue(atm_conn_t *c)
{
    if (c != NULL) {
        pthread_mutex_lock(&conn_free_lock);
        atm_list_push(conn_free_list, c);
        pthread_mutex_unlock(&conn_free_lock);
    }
}


static void
atm_conn_inactive(atm_conn_t *c)
{
    atm_event_t *e = c->event;
    if (e->active) {
        pthread_mutex_lock(&c->_mutex);
        if (e->active) {
            atm_event_inactive(e);
            atm_conn_free_enqueue(c);
        }
        pthread_mutex_unlock(&c->_mutex);
    }
    atm_conn_free_notify();
}


static void
atm_conn_inter_write(atm_conn_t *c, atm_uint_t wreqs, int wrmn)
{
    atm_event_t *e = NULL;

    pthread_mutex_lock(&c->_mutex);
    e = c->event;
    c->on_write = ATM_FALSE;
    /* wrmn > 0 means buf still have byte aval to write */
    if(c->write_reqs > wreqs || wrmn > 0) {
        if (e->active) {
            atm_event_add_notify(e, ATM_EVENT_WRITE);
        }
    }
    pthread_mutex_unlock(&c->_mutex);
}


static atm_bool_t
atm_conn_yield_write(atm_conn_t *c, atm_uint_t wreqs)
{
    atm_event_t *e = c->event;
    atm_bool_t res = ATM_FALSE;

    pthread_mutex_lock(&c->_mutex);
    if(c->write_reqs == wreqs || !e->active) {
        c->on_write = ATM_FALSE;
        res = ATM_TRUE;
    }
    pthread_mutex_unlock(&c->_mutex);
    return res;
}


static void
atm_conn_write_notify(atm_conn_t *c)
{
    atm_event_t *e = c->event;

    pthread_mutex_lock(&c->_mutex);
    if (e->active) {
        c->write_reqs += 1;
        if (c->on_write == ATM_FALSE) {
            atm_log("atm_conn_write_notify add");
            atm_event_add_notify(e, ATM_EVENT_WRITE); 
        } else {
            atm_log("atm_conn_wirte_notify req");
        }
    }
    pthread_mutex_unlock(&c->_mutex);
}


static atm_int_t
atm_conn_task_read_raw(atm_conn_t *conn)
{
    int ret = 0;
    atm_socket_t *srcsock = NULL;
    atm_buf_t *r_buf = NULL;

    srcsock = conn->sock;
    r_buf = conn->r_buf;   

    ret = atm_buf_read_sock(r_buf, srcsock);
    return ret;
}


static atm_int_t
atm_conn_task_read(atm_task_t *t)
{
    int ret = 0;
    atm_conn_t *conn;
    atm_sess_t *se = NULL;
    atm_event_t *e = NULL;

    conn = t->load;
    se = conn->session;
    e = conn->event;

    if (ATM_FALSE != e->active) {
        ret = atm_conn_task_read_raw(conn);
        if ((ret ==-1 && errno!=EAGAIN) || ret == 0) {
            atm_log("conn_task_read->ev inact");
            /* SAFE_FREE_TAG */
            atm_conn_inactive(conn);
            conn->on_read = ATM_FALSE;
            return ATM_ERROR;
        } else {
            atm_sess_process(se);
        }
    }
    
    /* restore read event */
    if (ATM_TRUE == e->active) {
        atm_log("conn_task_read->restore read");
        atm_event_add_notify(e,ATM_EVENT_READ);
    }

    if (e->active == ATM_FALSE) {
        conn->on_read = ATM_FALSE;
        atm_conn_free_notify();
    } else {
        conn->on_read = ATM_FALSE;
    }
    return ATM_OK;
}


static atm_int_t
atm_conn_task_write_raw(atm_conn_t *conn)
{
    int ret = 0;
    atm_socket_t *destsock = NULL;
    atm_buf_t *w_buf = NULL;

    destsock = conn->sock;
    w_buf = conn->w_buf;

    ret = atm_buf_write_sock(w_buf, destsock);
    return ret;
}


static atm_int_t
atm_conn_task_write(atm_task_t *t)
{
    int ret = 0;
    atm_conn_t *conn = NULL;
    atm_sess_t *se = NULL;
    atm_event_t *e = NULL;
    atm_uint_t wreqs = 0;
    atm_buf_t *w_buf = NULL;

    conn = t->load;
    se = conn->session;
    e = conn->event;
    w_buf = conn->w_buf;

    atm_log("atm_conn_task_write enter");
    while (ATM_FALSE != e->active) {
        wreqs = conn->write_reqs;
        ret = atm_conn_task_write_raw(conn);
        if (ret ==-1 && errno!=EAGAIN) {
            atm_log("atm_conn_task_write inact");
            /* SAFE_FREE_TAG */
            atm_conn_inactive(conn);
            conn->on_write = ATM_FALSE;
            return ATM_ERROR;
        } else if (ret == 0) {
            /* write is not aval, 
             * need wait another event */
            atm_log("atm_conn_task_write inter");
            /* need to have a rest */
            int wrmn = w_buf->aval;
            atm_conn_inter_write(conn,wreqs,wrmn);
            break;
        }
        /* to check if more work came in */
        if (atm_conn_yield_write(conn,wreqs)) {
            atm_log("atm_conn_task_write yield");
            break;
        }
    }

    if (e->active == ATM_FALSE) {
        conn->on_write = ATM_FALSE;
        atm_conn_free_notify();
    } else {
        conn->on_write = ATM_FALSE;
    }
    return ATM_OK;
}


static atm_conn_t *
atm_conn_new(atm_socket_t *cs)
{
    atm_conn_t * res = NULL;

    res = atm_alloc(sizeof(atm_conn_t));
    res->sock = cs;
    res->event = NULL;
    res->handle_read = atm_conn_handle_read;
    res->handle_write = atm_conn_handle_write;
    res->r_buf = atm_buf_new();
    res->w_buf = atm_buf_new();
    pthread_mutex_init(&res->_mutex,NULL);
    res->on_read = ATM_FALSE;
    res->on_write = ATM_FALSE;
    res->write_reqs = 0;
    return res; 
}


static atm_conn_listen_t *
atm_conn_listen_new(atm_socket_t *ss)
{
    atm_conn_listen_t * res = NULL;

    res = atm_alloc(sizeof(atm_conn_listen_t));
    res->ssck = ss;
    res->event = NULL;
    res->handle_accept = atm_conn_handle_accept;
    return res; 
}


static int
atm_conn_listen_tcp()
{
    /* TODO: need to fetch from config */
    atm_int_t port = 8088;
    char *bindaddr = "0.0.0.0";
    int backlog = 1024;

    atm_socket_t *ss = NULL;
    atm_conn_listen_t *tcpl = NULL; 
    int ret = -1;

    ss = atm_net_listen_tcp(port, 
            bindaddr, backlog);
    if (ss != NULL) {
        ret = atm_net_nonblock(ss, ATM_TRUE); 
        if (ret != ATM_OK) goto error;
        tcpl = atm_conn_listen_new(ss);
        /* register listen fd to epoll */
        atm_event_add_listen(tcpl);
    } else {
        atm_log_rout(ATM_LOG_ERROR,
            "atm_conn_listen_tcp, ss NULL " 
            "errno: %s",strerror(errno));
        goto error;
    }
    return ATM_OK;
error:
    atm_log_rout(ATM_LOG_ERROR,
        "atm_conn_listen_tcp, " 
        "errno: %s",strerror(errno));
    return ATM_ERROR;
}


static void
atm_conn_handle_accept(
        atm_event_t *listen_event)
{
    atm_int_t max = ATM_CONN_PERCALL_ACCEPTS;
    int interval = ATM_NET_DEFAULT_TCP_KEEPALIVE;

    atm_conn_listen_t *ls = NULL;
    atm_socket_t *ss = NULL;
    atm_socket_t *cs = NULL;
    atm_conn_t *conn = NULL;
    atm_sess_t *se = NULL;
    
    ls = listen_event->load;
    ss = ls->ssck;  
    while (--max) {
        cs = atm_net_accept(ss);
        if (cs == NULL) break;
        /*config conn socket*/
        atm_net_nonblock(cs, ATM_TRUE); 
        atm_net_nodelay(cs, ATM_TRUE); 
        atm_net_keepalive(cs, interval); 
        conn = atm_conn_new(cs);
        /* register conn to epoll */
        atm_event_add_conn(conn);
        se = atm_sess_new(conn);
        conn->session = se;
    }
}


static void
atm_conn_handle_read(atm_event_t *conn_event)
{
    atm_conn_t *conn = NULL;
    atm_task_t *task = NULL;
    if (conn_event != NULL) {
        conn = conn_event->load;
        /* SAFE_FREE_TAG */
        pthread_mutex_lock(&conn->_mutex);
        if (conn_event->active) {
            atm_event_del_event(conn_event, ATM_EVENT_READ);
            if (conn != NULL) {
                task = atm_task_new(
                        conn, 
                        atm_conn_task_read);
                atm_task_dispatch(task);
                conn->on_read = ATM_TRUE;
            }
        }
        pthread_mutex_unlock(&conn->_mutex);
    }
}


static void
atm_conn_handle_write(atm_event_t *conn_event)
{
    atm_conn_t *conn = NULL;
    atm_task_t *task = NULL;

    if (conn_event != NULL) {
        conn = conn_event->load;
        /* SAFE_FREE_TAG */
        pthread_mutex_lock(&conn->_mutex);
        if (conn_event->active) {
            atm_event_del_event(conn_event, ATM_EVENT_WRITE);
            if (conn != NULL) {
                task = atm_task_new(
                        conn, 
                        atm_conn_task_write);
                atm_task_dispatch(task);
                conn->on_write = ATM_TRUE;
            }
        }
        pthread_mutex_unlock(&conn->_mutex);
    }
}


/*
 * Public
 * */
void
atm_conn_init()
{
    atm_conn_listen_tcp();
    /* init conn free thread task */
    conn_free_list = atm_list_new(
            NULL, ATM_FREE_SHALLOW);
    pthread_mutex_init(&conn_free_lock, NULL);
}


void
atm_conn_free(void *conn)
{
    atm_conn_t *c = NULL;
    atm_buf_t *rbuf = NULL;
    atm_buf_t *wbuf = NULL;
    if (conn != NULL) {
        c = conn;
        rbuf = c->r_buf;
        wbuf = c->w_buf;

        atm_event_free(c->event);
        atm_socket_free(c->sock);
        atm_buf_free(rbuf);
        atm_buf_free(wbuf);
        pthread_mutex_destroy(&c->_mutex);
        atm_free(c);
    }
}


void
atm_conn_listen_free(void *listen)
{
    atm_conn_listen_t *l = NULL;
    if (listen != NULL) {
        l = listen;
        atm_event_free(l->event);
        atm_socket_free(l->ssck);
        atm_free(l);
    }
}


atm_str_t *
atm_conn_read_line(atm_conn_t *c)
{
    atm_str_t *res = NULL;
    atm_buf_t *r_buf = c->r_buf;
    char *s = atm_buf_read_line(r_buf);
    if (s != NULL) {
        res = atm_str_wrp(s);
    }
    return res; 
}


void
atm_conn_write(atm_conn_t *c, 
        void *src, atm_uint_t nbyte)
{
    atm_buf_t *w_buf = NULL; 
    if (c != NULL) {
        atm_log("atm_conn_write enter");
        w_buf = c->w_buf;
        atm_buf_write(w_buf,src,nbyte); 
        atm_conn_write_notify(c);
    }
}


void
atm_conn_write_str(atm_conn_t *c, atm_str_t *s)
{
    if (c != NULL) {
        atm_conn_write(c,s->val,s->len);
    }
}
