#include <atm_core.h>
/*
 * Private
 * */
static void
atm_conn_free_check(atm_conn_t *c, atm_bool_t *flag);
static void
atm_conn_free_notify();
static atm_int_t
atm_conn_free_func(atm_task_t *t);
static void
atm_conn_free_enqueue(atm_conn_t *c);
static void
atm_conn_inactive(atm_conn_t *c, atm_bool_t *flag);
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
static atm_list_t *free_list;
static pthread_mutex_t _mutex_free = PTHREAD_MUTEX_INITIALIZER;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_conn_free_check(atm_conn_t *conn, atm_bool_t *flag)
{
    atm_conn_t *c = conn;
    /* TODO, volative can  discard compile optz? how to check it*/
    volatile atm_event_t *e = c->event;
    /* make sure the active stat no change during confirmation */
    pthread_mutex_lock(&c->_mutex);
    if (e->active == ATM_FALSE) {
        pthread_mutex_lock(&_mutex_free);
            *flag = ATM_FALSE;
            atm_conn_free_notify();
            /* make sure the _mutex will not be destoried
             * by the free conn func, is exsit*/
            pthread_mutex_unlock(&c->_mutex);
        pthread_mutex_unlock(&_mutex_free);
    } else {
        *flag = ATM_FALSE;
        pthread_mutex_unlock(&c->_mutex);
    }
}


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
    atm_conn_t *c;
    atm_conn_t *unproc;
    atm_sess_t *sess;
    atm_list_t *ls;

    unproc = NULL;
    pthread_mutex_lock(&_mutex_free);
    ls = free_list;
    while ((c = atm_list_lpeek(ls)) != unproc) {
        c = atm_list_lpop(ls);
        /* do normal conn check */
        if (!c->on_write && !c->on_read) {
            sess = c->session;
            atm_sess_free(sess);
        } else {
            if (unproc == NULL) unproc = c;
            atm_list_push(ls, c);
        }
    }
    pthread_mutex_unlock(&_mutex_free);
    return ATM_OK;
}


static void
atm_conn_free_enqueue(atm_conn_t *c)
{
    if (c != NULL) {
        pthread_mutex_lock(&_mutex_free);
        atm_list_push(free_list, c);
        pthread_mutex_unlock(&_mutex_free);
    }
}


static void
atm_conn_inactive(atm_conn_t *c, atm_bool_t *flag)
{
    atm_event_t *e = c->event;
    if (e->active) {
        pthread_mutex_lock(&c->_mutex);
        if (e->active) {
            atm_event_inactive(e);
            *flag = ATM_FALSE;
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
    atm_socket_t *srcsock;
    atm_buf_t *r_buf;

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
    atm_sess_t *se;
    atm_event_t *e;

    conn = t->load;
    se = conn->session;
    e = conn->event;

    if (ATM_FALSE != e->active) {
        ret = atm_conn_task_read_raw(conn);
        if ((ret ==-1 && errno!=EAGAIN) || ret == 0) {
            atm_log("conn_task_read->ev inact");
            atm_conn_inactive(conn, &conn->on_read);
            return ATM_ERROR;
        } else {
            ret = atm_sess_process(se);
            if (ret == ATM_ERROR)
                atm_conn_inactive(conn, &conn->on_read);
        }
    }

    /* restore read event */
    if (ATM_TRUE == e->active) {
        atm_log("conn_task_read->restore read");
        atm_event_add_notify(e,ATM_EVENT_READ);
    }

    atm_conn_free_check(conn,&conn->on_read);
    return ATM_OK;
}


static atm_int_t
atm_conn_task_write_raw(atm_conn_t *conn)
{
    int ret = 0;
    atm_socket_t *destsock;
    atm_buf_t *w_buf;

    destsock = conn->sock;
    w_buf = conn->w_buf;

    ret = atm_buf_write_sock(w_buf, destsock);
    return ret;
}


static atm_int_t
atm_conn_task_write(atm_task_t *t)
{
    int ret = 0;
    atm_conn_t *conn;
    atm_event_t *e;
    atm_uint_t wreqs;
    atm_buf_t *w_buf;

    conn = t->load;
    e = conn->event;
    w_buf = conn->w_buf;

    atm_log("#####atm_conn_task_write enter task %p", t);
    while (ATM_FALSE != e->active) {
        wreqs = conn->write_reqs;
        ret = atm_conn_task_write_raw(conn);
        if (ret ==-1 && errno!=EAGAIN) {
            atm_log("atm_conn_task_write inact");
            atm_conn_inactive(conn, &conn->on_write);
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

    atm_conn_free_check(conn,&conn->on_write);
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
    atm_config_t *conf = atm_ctx->config;
    atm_int_t port = conf->port;
    int backlog = conf->tcpbacklog;
    atm_socket_t *ss;
    atm_conn_listen_t *tcpl;

    int ret;
    atm_uint_t j;
    atm_str_t bindaddr;
    atm_str_t defaultbind;

    /* set default */
    if(conf->bindaddr_count==0) {
        if (conf->bindaddr == NULL) {
            defaultbind = atm_str_new("0.0.0.0");
            conf->bindaddr = &defaultbind;
            conf->bindaddr_count = 1;
        } else {
            atm_log_rout(ATM_LOG_FATAL, "bindaddr corrupted");
            exit(ATM_ERROR);
        }
    }

    /* one bindaddr setup begin */
    for (j=0;j<conf->bindaddr_count;j++) {
        bindaddr = conf->bindaddr[j];
        ss = atm_net_listen_tcp(port,
                bindaddr, backlog);
        if (ss != NULL) {
            ret = atm_net_nonblock(ss, ATM_TRUE);
            if (ret != ATM_OK) continue;
            tcpl = atm_conn_listen_new(ss);
            /* register listen fd to epoll */
            atm_event_add_listen(tcpl);
        } else {
            atm_log_rout(ATM_LOG_ERROR,
                "atm_conn_listen_tcp, ss NULL "
                "errno: %s",strerror(errno));
        }
    }
    /* one bindaddr setup end */

    return ATM_OK;
}


static void
atm_conn_handle_accept(
        atm_event_t *listen_event)
{
    atm_conn_listen_t *ls;
    atm_socket_t *ss;
    atm_socket_t *cs;
    atm_conn_t *conn;
    atm_sess_t *se;

    atm_int_t max = ATM_CONN_PERCALL_ACCEPTS;
    int interval = ATM_NET_DEFAULT_TCP_KEEPALIVE;

    ls = listen_event->load;
    ss = ls->ssck;
    while (max--) {
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
    atm_conn_t *conn;
    atm_task_t *task;

    if (conn_event != NULL) {
        conn = conn_event->load;
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
    atm_conn_t *conn;
    atm_task_t *task;

    if (conn_event != NULL) {
        conn = conn_event->load;
        pthread_mutex_lock(&conn->_mutex);
        if (conn_event->active) {
            atm_event_del_event(conn_event, ATM_EVENT_WRITE);
            if (conn != NULL) {
                task = atm_task_new(
                        conn,
                        atm_conn_task_write);
                atm_task_dispatch(task);
                atm_log("########write_task dispatch %p", task);
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
    /* init conn free list */
    free_list = atm_list_new(
            NULL, ATM_FREE_SHALLOW);
}


void
atm_conn_free(void *conn)
{
    atm_conn_t *c;
    atm_buf_t *rbuf;
    atm_buf_t *wbuf;

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
    atm_conn_listen_t *l;

    if (listen != NULL) {
        l = listen;
        atm_event_free(l->event);
        atm_socket_free(l->ssck);
        atm_free(l);
    }
}


atm_int_t
atm_conn_read(atm_conn_t *c,
        void *dest, atm_uint_t nbyte)
{
    atm_buf_t *r_buf = c->r_buf;
    return atm_buf_read(r_buf,dest,nbyte);
}


atm_str_t
atm_conn_read_line(atm_conn_t *c)
{
    atm_str_t res = NULL;

    atm_buf_t *r_buf = c->r_buf;
    char *s = atm_buf_read_line(r_buf);
    if (s != NULL) {
        res = atm_str_new(s);
    }
    return res;
}


void
atm_conn_write(atm_conn_t *c,
        void *src, atm_uint_t nbyte)
{
    atm_buf_t *w_buf;

    if (c != NULL) {
        w_buf = c->w_buf;
        atm_buf_write(w_buf,src,nbyte);
        atm_conn_write_notify(c);
    }
}


void
atm_conn_write_str(atm_conn_t *c, atm_str_t s)
{
    atm_uint_t len;
    if (c != NULL) {
        len = atm_str_len(s);
        atm_conn_write(c,s,len);
    }
}
