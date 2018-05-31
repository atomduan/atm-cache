
/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/28/2017 11:04:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <misc_utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sched.h>

static void *task_one(void *);
static void *task_two(void *);

static pthread_mutex_t lock;

int main(int argc, char **argv) {
    pthread_t tid_one;
    pthread_t tid_two;

    pthread_mutex_init(&lock, NULL);
    pthread_create(&tid_one, NULL, task_one, NULL);
    pthread_create(&tid_two, NULL, task_two, NULL);

    pthread_join(tid_one, NULL);
    pthread_join(tid_two, NULL);
    return 0;
}

static void *task_one(void *argv) {
    int i = 0;
    unsigned int tid = pthread_self(); 
    for (;i<10;++i) {
        pthread_mutex_lock(&lock);
        usleep(1000*1000);
        printf("thread for task one %u\n", tid);
        sched_yield();
        pthread_mutex_unlock(&lock);
        //pthread_yield();
    }
}
static void *task_two(void *argv) {
    int i = 0;
    unsigned int tid = pthread_self();
    for (;i<10;++i) {
        pthread_mutex_lock(&lock);
        usleep(1000*1000);
        printf("thread for task_two %u\n", tid);
        sched_yield();
        pthread_mutex_unlock(&lock);
    }
}
