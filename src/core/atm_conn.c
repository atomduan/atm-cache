#include <atm_core.h>

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
    atm_socket_t *so = NULL;
    atm_int_t port = 80;
    char *bindaddr = "0.0.0.0";
    int backlog = 1024;
    atm_conn_listen_t *tcpl = NULL; 

    so = atm_net_listen_tcp(port, 
            bindaddr, backlog);
    if (so != NULL) {
        ret = atm_net_nonblock(so, ATM_TRUE); 

        if (ret != ATM_OK)
            goto error;

        tcpl = atm_conn_listen_new(so);
        /* register listen fd to epoll */
        atm_event_add_listen(tcpl);
    } else {
        atm_log_rout(ATM_LOG_ERROR,
            "atm_conn_listen_tcp, so NULL " 
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
atm_conn_handle_accept(
        atm_event_t *listen_event)
{
    atm_int_t max = ATM_CONN_PERCALL_ACCEPTS;
    atm_socket_t *ss;
    atm_socket_t *s;
    atm_conn_t *conn = NULL;
    atm_conn_listen_t *ls = NULL;
    
    ls = listen_event->load;
    ss = ls->ssck;  
    while (--max) {
        s = atm_net_accept(ss);
        if (s != NULL) {
            atm_net_nonblock(s, ATM_TRUE); 
            conn = atm_conn_new(s);
            /* register conn to epoll */
            atm_event_add_conn(conn);
        }
    }
}


void
atm_conn_handle_read(
        atm_event_t *conn_event)
{

}


void
atm_conn_handle_write(
        atm_event_t *conn_event)
{

}
