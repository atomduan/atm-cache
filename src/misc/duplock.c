#include <atm_linux_config.h>
#include <misc_utils.h>

/* test the memory align figure */
int main(int argc, char **argv)
{
    int ret = 0;
    pthread_rwlock_t    rwlk;
    pthread_rwlock_init(&rwlk, NULL);

    pthread_mutex_t     mutex;
    pthread_mutex_init(&mutex, NULL);

    /* the under will core dump on second unlock
     * */
    //pthread_rwlock_wrlock(&rwlk);
    //printf("enter in first wrlock");
    //pthread_rwlock_wrlock(&rwlk);
    //printf("enter in second wrlock");
    //pthread_rwlock_unlock(&rwlk);
    //printf("leave second wrlock");
    //pthread_rwlock_unlock(&rwlk);
    //printf("leave first wrlock");

    /* the under will run normally
     * */
    ret = pthread_rwlock_wrlock(&rwlk);
    printf("enter in first wrlock, ret is %d\n", ret);
    ret = pthread_rwlock_unlock(&rwlk);
    printf("leave first wrlock, ret is %d\n", ret);

    /* the under will core dump on second unlock
     * */
    //ret = pthread_rwlock_wrlock(&rwlk);
    //printf("enter in first wrlock, ret is %d\n", ret);

    //ret = pthread_rwlock_rdlock(&rwlk);
    //printf("enter in first rdlock, ret is %d\n", ret);
    ///* ret is 35, error msg is Resource deadlock avoided */
    //printf("error is %s\n", strerror(ret));

    //ret = pthread_rwlock_unlock(&rwlk);
    //printf("leave first rdlock, ret is %d\n", ret);

    ///* a core will happens here*/
    //ret = pthread_rwlock_unlock(&rwlk);
    //printf("leave first wrlock, ret is %d\n", ret);

    /* a dead lock will happed on second lock */
    //ret = pthread_mutex_lock(&mutex);
    //printf("enter in first mutex, ret is %d\n", ret);
    //ret = pthread_mutex_lock(&mutex);
    //printf("enter in second mutex, ret is %d\n", ret);
    //ret = pthread_mutex_unlock(&mutex);
    //printf("leave first mutex, ret is %d\n", ret);

    /* run normally */
    ret = pthread_mutex_lock(&mutex);
    printf("enter in first mutex, ret is %d\n", ret);
    ret = pthread_mutex_unlock(&mutex);
    printf("leave first mutex, ret is %d\n", ret);

    pthread_mutex_destroy(&mutex);
    pthread_rwlock_destroy(&rwlk);
    return 0;
}
