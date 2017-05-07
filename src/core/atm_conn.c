#include <atm_core.h>
/*
 * Private
 * */
static void 
atm_conn_post_proc(
        atm_event_t *conn_event);

static void
atm_conn_listen_post_proc(
        atm_event_t *listen_event);

static atm_int_t 
atm_conn_task_read_raw(atm_conn_t *conn); 

static atm_int_t 
atm_conn_task_read(atm_conn_t *conn); 

static atm_int_t
atm_conn_task_write_raw(atm_conn_t *conn);

static atm_int_t
atm_conn_task_write(atm_conn_t *conn); 

static atm_conn_t *
atm_conn_new(atm_socket_t *so);

static atm_conn_listen_t *
atm_conn_listen_new(atm_socket_t *so);

static int
atm_conn_listen_tcp();

static void
atm_conn_listen_free(void *listen);

static void
atm_conn_handle_accept(atm_event_t *listen_event);

static void
atm_conn_handle_read(atm_event_t *conn_event);

static void
atm_conn_handle_write(atm_event_t *conn_event);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_conn_post_proc(
        atm_event_t *conn_event)
{
    atm_event_t *e = NULL;
    atm_conn_t *c = NULL;
    atm_sess_t *se = NULL;

    e = conn_event;
    c = e->load;
    if (!e->r_act || !e->w_act) {
        se = c->session;
        atm_sess_free(se); 
    }
}


static void
atm_conn_listen_post_proc(
        atm_event_t *listen_event)
{
    atm_event_t *e = NULL;
    atm_conn_listen_t *l = NULL;

    e = listen_event;
    l = e->load;
    if (!e->r_act) {
        atm_conn_listen_free(l);        
    }
}


static atm_int_t
atm_conn_task_read_raw(atm_conn_t *conn)
{
    int ret = 0;
    int total = 0;

    atm_socket_t *srcfd = NULL;
    atm_sess_t *se = NULL;
    atm_buf_t *rbuf = NULL;

    srcfd = conn->sock;
    se = conn->session;

    rbuf = se->r_buf;   
    while (ATM_TRUE) {
        ret = atm_buf_writef(rbuf, srcfd);
        if (ret > 0) {
            total += ret;
            continue;
        }
        if (ret < 0) {
            return ret;
        }
        break;
    }
    return total;
}


static atm_int_t
atm_conn_task_read(atm_conn_t *conn)
{
    int ret = 0;
    atm_sess_t *se = NULL;
    atm_event_t *e = NULL;

    se = conn->session;
    e = conn->event;

    ret = atm_conn_task_read_raw(conn);
    if ((ret ==-1 && errno!=EAGAIN) || ret == 0) {
        e->r_act = ATM_FALSE;
        return ATM_OK;
    }
    atm_sess_process(se);
    return ATM_OK;
}


static atm_int_t
atm_conn_task_write_raw(atm_conn_t *conn)
{
    int ret = 0;
    int total = 0;

    atm_socket_t *destfd = NULL;
    atm_sess_t *se = NULL;
    atm_buf_t *wbuf = NULL;

    destfd = conn->sock;
    se = conn->session;

    wbuf = se->w_buf;
    while (ATM_TRUE) {
        ret = atm_buf_readf(wbuf, destfd);
        if (ret > 0) {
            total += ret;
            continue;
        } else 
        if (ret < 0) {
            return ret;
        }
        break;
    }
    return total;
}


static atm_int_t
atm_conn_task_write(atm_conn_t *conn)
{
    int ret = 0;
    atm_sess_t *se = NULL;
    atm_event_t *e = NULL;

    se = conn->session;
    e = conn->event;

    ret = atm_conn_task_write_raw(conn);
    if ((ret ==-1 && errno!=EAGAIN) || ret == 0) {
        e->w_act = ATM_FALSE;
    }
    return ATM_OK;
}


static atm_conn_t *
atm_conn_new(atm_socket_t *cs)
{
    atm_conn_t * res = NULL;
    res = atm_alloc(sizeof(atm_conn_t));
    res->sock = cs;
    /* should be prop by event module */
    res->event = NULL;
    res->handle_read = atm_conn_handle_read;
    res->handle_write = atm_conn_handle_write;
    res->post_proc = atm_conn_post_proc;
    return res; 
}


static atm_conn_listen_t *
atm_conn_listen_new(atm_socket_t *ss)
{
    atm_conn_listen_t * res = NULL;
    res = atm_alloc(sizeof(atm_conn_listen_t));
    res->ssck = ss;
    /* should be prop by event module */
    res->event = NULL;
    res->handle_accept = atm_conn_handle_accept;
    res->post_proc = atm_conn_listen_post_proc;
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
atm_conn_handle_read(
        atm_event_t *conn_event)
{
    atm_conn_t *conn = NULL;

    conn = conn_event->load;
    if (conn != NULL) {
        atm_conn_task_read(conn); 
    }
}


static void
atm_conn_handle_write(
        atm_event_t *conn_event)
{
    atm_conn_t *conn = NULL;

    conn = conn_event->load;
    if (conn != NULL) {
        atm_conn_task_write(conn); 
    }
}


/*
 * Public
 * */
void
atm_conn_init()
{
    atm_conn_listen_tcp();
}


void
atm_conn_free(void *conn)
{
    atm_conn_t *c = NULL;
    if (conn != NULL) {
        c = conn;
        atm_event_free(c->event);
        atm_socket_free(c->sock);
        atm_free(c);
    }
}


void
atm_conn_wnotify(void *conn)
{
    atm_conn_t *c = conn;
    atm_event_add_event(c->event, 
            ATM_EVENT_WRITE);
}
