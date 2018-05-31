#include <misc_utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "test.h"

static void sig_cld_handler(int sig) {
    pid_t pid;
    int status;
    int delay = 10;
    printf("enter cld handler\n");
    printf("re hook the sig_cld_handler again\n");
    if (signal(SIGCLD, sig_cld_handler) == SIG_ERR) {
        printf("signal error\n");
        exit(0);
    }

    printf("wait pid.....\n");
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("wait a sub process\n");
    }

    for (;delay > 0; --delay) {
        printf("process OK wait for 10 sec in handler....[%d]\n", delay);
        sleep(1);
    }
}

static void child_process() {
    int i = 0;
    //child process
    for(;i<10;++i) {
        printf("child process living loop [%d]\n", i);
        sleep(1);
    }
    printf("child process exit......\n");
    exit(0);
}

int main(int argc, char **argv) {
    pid_t pid;
    int i = 0;

    if (signal(SIGCLD, sig_cld_handler) == SIG_ERR) {
        printf("sgnal error in in init\n");
        exit(0);
    }
    printf("default pid value is %d\n", pid);
    for (;i<10;++i) {
        if ((pid = fork()) < 0) {
            printf("fork error");
        } else if (pid == 0) {
            child_process();
        }
    }
    printf("this is a special point to execute.......");
    //parent process
    for(;;) {
        printf("main loop....\n");
        sleep(1);
    }
    printf("main loop exit....\n");
    return 0;
}
