#include <atm_core.h>
       

static atm_int_t 
atm_conn_task_read(atm_task_t *self); 

static atm_int_t
atm_conn_task_write(atm_task_t *self); 

static atm_conn_t *
atm_conn_new(atm_socket_t *so);

static atm_conn_listen_t *
atm_conn_listen_new(atm_socket_t *so);

static int
atm_conn_listen_tcp();


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */

static atm_int_t
atm_conn_task_read(atm_task_t *self)
{
    atm_conn_t *conn = NULL;
    atm_socket_t *src = NULL;
    atm_session_t *se = NULL;
    atm_buf_t *rbuf = NULL;

    int len = ATM_BUF_DEFAULT_LEN;

    conn = self->load;
    src = conn->sock;
    se = conn->session;


    rbuf = se->r_buf;   
    atm_buf_writef(rbuf, src, len);

    atm_session_process(se);
    return 0;
}


static atm_int_t
atm_conn_task_write(atm_task_t *self)
{
    atm_conn_t *conn = NULL;
    atm_session_t *se = NULL;
    atm_socket_t *dest = NULL;
    atm_buf_t *wbuf = NULL;

    int len = ATM_BUF_DEFAULT_LEN;

    conn = self->load;
    dest = conn->sock;
    se = conn->session;

    wbuf = se->w_buf;
    atm_buf_readf(wbuf, dest, len);
    return 0;
}


static atm_conn_t *
atm_conn_new(atm_socket_t *so)
{
    atm_conn_t * res = NULL;
    res = atm_alloc(sizeof(atm_conn_t));
    res->sock = so;
    /* should be prop by event module */
    res->event = NULL;
    res->handle_read = atm_conn_handle_read;
    res->handle_write = atm_conn_handle_write;
    return res; 
}


static atm_conn_listen_t *
atm_conn_listen_new(atm_socket_t *so)
{
    atm_conn_listen_t * res = NULL;
    res = atm_alloc(sizeof(atm_conn_listen_t));
    res->ssck = so;
    /* should be prop by event module */
    res->event = NULL;
    res->handle_accept = atm_conn_handle_accept;
    return res; 
}


static int
atm_conn_listen_tcp()
{
    int ret = -1;
    atm_socket_t *ss = NULL;
    atm_conn_listen_t *tcpl = NULL; 

    /* TODO: need to fetch from config */
    atm_int_t port = 80;
    char *bindaddr = "0.0.0.0";
    int backlog = 1024;

    ss = atm_net_listen_tcp(port, 
            bindaddr, backlog);
    if (ss != NULL) {
        ret = atm_net_nonblock(ss, ATM_TRUE); 

        if (ret != ATM_OK)
            goto error;

        tcpl = atm_conn_listen_new(ss);
        /* register listen fd to epoll */
        atm_event_add_listen(tcpl);
    } else {
        atm_log_rout(ATM_LOG_ERROR,
            "atm_conn_listen_tcp, ss NULL " 
            "errno: %s",strerror(errno));
        goto error;
    }
error:
    atm_log_rout(ATM_LOG_ERROR,
        "atm_conn_listen_tcp, " 
        "errno: %s",strerror(errno));
    return ATM_ERROR;
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
        atm_free(conn);
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
        atm_free(listen);
    }
}


void
atm_conn_handle_accept(
        atm_event_t *listen_event)
{
    atm_int_t max = ATM_CONN_PERCALL_ACCEPTS;
    atm_socket_t *ss = NULL;
    atm_socket_t *cs = NULL;
    atm_conn_t *conn = NULL;
    atm_conn_listen_t *ls = NULL;
    atm_session_t *se = NULL;
    int interval = ATM_NET_DEFAULT_TCP_KEEPALIVE;
    
    ls = listen_event->load;
    ss = ls->ssck;  
    while (--max) {
        cs = atm_net_accept(ss);
        if (cs != NULL) {
            atm_net_nonblock(cs, ATM_TRUE); 
            atm_net_nodelay(cs, ATM_TRUE); 
            atm_net_keepalive(cs, interval); 
            conn = atm_conn_new(cs);
            /* register conn to epoll */
            atm_event_add_conn(conn);
            se = atm_session_new(conn);
            conn->session = se;
        }
    }
}


void
atm_conn_handle_read(
        atm_event_t *conn_event)
{
    atm_conn_t *conn = NULL;
    atm_task_t *t = NULL;

    conn = conn_event->load;
    if (conn != NULL) {
       t = atm_task_new(conn, 
               atm_conn_task_read); 
       /* Queue the read task */
       atm_task_dispatch(t);
    }
}


void
atm_conn_handle_write(
        atm_event_t *conn_event)
{
    atm_conn_t *conn = NULL;
    atm_task_t *t = NULL;

    conn = conn_event->load;
    if (conn != NULL) {
       t = atm_task_new(conn, 
               atm_conn_task_write); 
       /* Do not using multi thread */
       t->run(t);
    }
}
