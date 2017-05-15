#include <atm_core.h>
/*
 * Private
 * */
static void 
atm_task_notify();

static void 
atm_task_process_tasks(char *notify);

static void 
atm_task_notify_recv(atm_event_t *ev);

static void
atm_task_nonblock_fd(int fd);

static void
atm_task_pipe_init();


static atm_list_t *tasks;


static int pipe_recv_fd = -1;
static int pipe_sent_fd = -1;


static atm_T_t ATM_TASK_TYPE = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    atm_task_free,
};

static atm_T_t *ATM_TASK_T = &ATM_TASK_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void 
atm_task_notify()
{
    char buf[1];

    buf[0] = 't';
    if (write(pipe_sent_fd,buf,1)!=1) {
        atm_log_rout(ATM_LOG_ERROR, 
                "task notify to pipe fail");
    }
}


static void 
atm_task_process_tasks(char *notify)
{
    atm_task_t *t = NULL;
    switch (notify[0]) {
        case 't':
            /* this is a single thread impl */
            /* TODO */
            while (ATM_TRUE) {
                t=atm_list_lpop(tasks);
                if (t == NULL) break;
                t->run(t);
                atm_task_free(t);
            }
        break;
    }
}


static void 
atm_task_notify_recv(atm_event_t *ev)
{
    int processed = 0;
    int ret = 0;
    char buf[1];
    int fd = pipe_recv_fd;

    while (ATM_TRUE) {
        ret = read(fd, buf, 1);
        if (ret > 0) {
            if (!processed) {
                atm_task_process_tasks(buf);             
                processed++;
            }
            /* flush all buf */
            continue;
        } else {
            break;
        }
    }
}


static void
atm_task_nonblock_fd(int fd)
{
    int flags;
    if ((flags=fcntl(fd, F_GETFL))==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_GETFL): %s", 
            strerror(errno));
        exit(1);
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd,F_SETFL,flags)==-1) {
        atm_log_rout(ATM_LOG_FATAL,
            "fcntl(F_SETFL,O_NONBLOCK): %s", 
            strerror(errno));
        exit(1);
    }
}


static void
atm_task_pipe_init()
{
    int fds[2];
    atm_event_t *pe = NULL;
    int events = ATM_EVENT_NONE;

    if (pipe(fds)) {
        atm_log_rout(ATM_LOG_FATAL, 
                "can not create notify pipe");
        exit(1);
    }

    pipe_recv_fd = fds[0];
    atm_task_nonblock_fd(pipe_recv_fd);
    pipe_sent_fd = fds[1];
    atm_task_nonblock_fd(pipe_sent_fd);

    pe = atm_event_new(
            NULL,
            pipe_recv_fd,
            atm_task_notify_recv,
            NULL, 
            NULL); 

    events = EPOLLIN|EPOLLHUP|EPOLLET;
    atm_log("task initial events mask %u", events);
    atm_event_add_event(pe, events);
}


/*
 * Public
 * */
void 
atm_task_init()
{
    /* Yes we actrally need a thread pool */
    tasks = atm_list_new(
            ATM_TASK_T, 
            ATM_FREE_DEEP);
    /* Pipe the task event and reg to epoll*/
    atm_task_pipe_init();
}


atm_task_t * 
atm_task_new(void *load, 
        atm_int_t (*run)(atm_task_t *self))
{
    atm_task_t *t = NULL;
    t = atm_alloc(sizeof(atm_task_t));
    t->load = load;
    t->run = run;
    return t;
}


void 
atm_task_free(void *task)
{
    atm_task_t *t = task;
    if (t != NULL) {
        atm_free(t);
    }
}


void 
atm_task_dispatch(atm_task_t *task)
{
    atm_task_t *t = task;
    if (t != NULL) {
        atm_list_push(tasks, t);
        atm_task_notify();
    }
}
