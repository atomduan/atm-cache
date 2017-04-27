#include <atm_core.h>


/*
 * Private
 * */
static int
atm_net_accept_raw(atm_socket_t *ssock, 
        atm_socket_t *csock);

static atm_socket_t *
atm_net_socket_new(int fd); 

static int
atm_net_set_reuse_addr(int sockfd);

static int 
atm_net_listen_raw(int sockfd, 
        struct sockaddr *sa, 
        socklen_t len, int backlog);

static int
atm_net_listen_tcp_raw(int port, 
        char *bindaddr, int backlog);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static int
atm_net_accept_raw(atm_socket_t *ssock, 
        atm_socket_t *csock)
{
    int fd;
    struct sockaddr_in *s;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    int port;
    int ss;
   
    ss = ssock->fd;
    char ip[ATM_NET_IP_STR_LEN];
    atm_uint_t ip_len = sizeof(ip);

    while(ATM_TRUE) {
        fd = accept(ss, (struct sockaddr*)&sa, &salen);
        if (fd == ATM_NET_ERR_FD) {
            if (errno == EINTR) continue;
            else {
                atm_log_rout(ATM_LOG_ERROR,
                    "accept: %s",strerror(errno));
                break;
            }
        }
        break;
    }

    if (fd == ATM_NET_ERR_FD)
        return ATM_ERROR;
    if (sa.ss_family == AF_INET) {
        s = (struct sockaddr_in *)&sa;
        inet_ntop(AF_INET,(void *)&(s->sin_addr),ip,ip_len);
        port = ntohs(s->sin_port); 
        atm_log("Accept: %s:%d", ip, port);
        csock->src_ip = atm_str_new(ip);
        csock->src_port = port;
    }
    csock->fd = fd;
    return ATM_OK;
}


static atm_socket_t *
atm_net_socket_new(int fd)
{
    atm_socket_t *res = NULL;
    res = atm_alloc(sizeof(atm_socket_t));
    res->fd = fd;
    return res;
}


static int 
atm_net_listen_raw(int sockfd, 
        struct sockaddr *sa, 
        socklen_t len, int backlog)
{
    if (bind(sockfd,sa,len) == -1) {
        atm_log_rout(ATM_LOG_ERROR, 
            "bind: %s", strerror(errno));
        return ATM_ERROR;
    }

    if (listen(sockfd, backlog) == -1) {
        atm_log_rout(ATM_LOG_ERROR,
            "listen: %s", strerror(errno));
        return ATM_ERROR;
    }
    return ATM_OK;

}


static int
atm_net_set_reuse_addr(int sockfd)
{
    int yes = 1;
    int ret = setsockopt(sockfd, 
            SOL_SOCKET, 
            SO_REUSEADDR, 
            &yes, sizeof(yes));

    if (ret == -1) {
        atm_log_rout(ATM_LOG_ERROR, 
            "setsockopt SO_REUSEADDR: %s", 
            strerror(errno));
        return ATM_ERROR;
    }
    return ATM_OK;
}


static int
atm_net_listen_tcp_raw(int port, 
        char *bindaddr, int backlog)
{
    int             sockfd = -1;
    int             ret = 0;
    char            port_str[6];
    struct addrinfo hints,*servinfo,*p;

    snprintf(port_str,6,"%d",port);
    memset(&hints,0,sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    /* TODO No effect if bindaddr != NULL */
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(bindaddr,
            port_str, &hints, &servinfo);

    if (ret != ATM_OK) {
        atm_log_rout(ATM_LOG_ERROR, 
            "listen getaddrinfo error ret[%d]", ret);
        return ATM_NET_ERR_FD;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family,
            p->ai_socktype, p->ai_protocol);

        if (sockfd == ATM_NET_ERR_FD)
            continue;
        ret = atm_net_set_reuse_addr(sockfd);
        if (ATM_OK != ret) goto error;

        ret = atm_net_listen_raw(sockfd,
            p->ai_addr, p->ai_addrlen, backlog);
        if (ATM_OK != ret) goto error;

        goto end;
    }

    if (p == NULL) {
        atm_log_rout(ATM_LOG_ERROR,
            "socket bind error, "
            "errno: %s",strerror(errno));
        goto error;
    }
error:
    if (sockfd != ATM_NET_ERR_FD) {
        close(sockfd);
    }
    sockfd = ATM_NET_ERR_FD;
end:
    freeaddrinfo(servinfo);
    return sockfd;
}


/*
 * Public
 * */

void
atm_net_socket_free(void *sock)
{
    atm_socket_t *s = sock;
    atm_str_free(s->src_ip);
    atm_free(s);
}


atm_socket_t *
atm_net_listen_tcp(int port, 
        char *bindaddr, int backlog)
{
    atm_socket_t *res = NULL;
    int s = ATM_NET_ERR_FD;
    s = atm_net_listen_tcp_raw(port, bindaddr, backlog);
    if (s != ATM_NET_ERR_FD) {
       res = atm_net_socket_new(s); 
       res->fd = s;
    }
    return res;
}


int       
atm_net_nonblock(atm_socket_t *s,
        atm_bool_t non_block)
{
    int flags;
    int fd = s->fd;    

    /* Set the socket blocking 
     * (if non_block is zero) or non-blocking.
     * Note that fcntl(2) for F_GETFL 
     * and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags=fcntl(fd, F_GETFL))==-1) {
        atm_log_rout(ATM_LOG_ERROR,
            "fcntl(F_GETFL): %s", 
            strerror(errno));
        return ATM_ERROR;
    }

    if (non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd,F_SETFL,flags)==-1) {
        atm_log_rout(ATM_LOG_ERROR,
            "fcntl(F_SETFL,O_NONBLOCK): %s", 
            strerror(errno));
        return ATM_ERROR;
    }
    return ATM_OK;
}


atm_socket_t *
atm_net_accept(atm_socket_t *ss)
{
    int ret = -1;
    atm_socket_t * res = NULL;
    res = atm_net_socket_new(ATM_NET_ERR_FD);
    ret = atm_net_accept_raw(ss, res);
    if (ret != ATM_OK) {
        atm_free(res);
        res = NULL;
    }
    return res;
}
