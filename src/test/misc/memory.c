#include <atm_linux_config.h>
#include <misc_utils.h>

#include <sys/ipc.h>
#include <sys/shm.h>

/*
 * for top command
 * VIRT = RES + SWAP + Unalloc
 * RES is same to /proc/$pid/smap's Rss
 *
 * RES = SHR(phycial mem part) + Others(physical part)
 * SHR is share memory
 * Others is mem alloc from malloc etc
 *
 * RES is pure physical mem
 */
int main(int argc, char **argv)
{
    int count = 1;
    int shmid;
    void *p;
    void *s;
    /* malloc will not afact Rss immediately */
    //p = malloc(4096*4096);
    /* calloc will afact Rss immediately */
    p = calloc(4096,4096);

    /* share memory */
    shmid = shmget(IPC_PRIVATE,4096*1024,IPC_CREAT|0666);
    if (shmid < 0) {
        printf("get shm ipc_id error\n");
        exit(-1);
    }
    s = shmat(shmid,NULL,0);
    printf("share %p\n",s);

    memset(s,0,4096*1024);

    while (count--) {
        sleep(1);
    }
    printf("%p\n",p);
    printf("%p\n",s);
    return 0;
}
