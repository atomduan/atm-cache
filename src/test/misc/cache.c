#include <atm_linux_config.h>
#include <misc_utils.h>

/* the cpu cache line can
 * heavyly impact the mm copy profermence
 * more than 100 times difference
 * SO, to zero-copy as possible as we can.
 */
static void
atm_sys_cache_line_profile()
{
    printf("atm_sys_cache_line_profile......\n");
    char src[65536], dest[65536];
    atm_timespec_t  ts[2];
    unsigned long timediff = 0;

    timediff = 0;
    atm_time_mono(&ts[0]);
    for (int j = 0; j<10000; ++j) {
        /* DUMBCOPY */
        for(int i=0; i<65536; i++) dest[i] = src[i];
    }
    atm_time_mono(&ts[1]);
    timediff = atm_time_mval(&ts[1], &ts[0]);
    printf("memcpy without cache line: %lu ms\n", timediff);


    timediff = 0;
    atm_time_mono(&ts[0]);
    for (int j = 0; j<10000; ++j) {
        /* SMARTCOPY */
        memcpy(dest, src, 65536);
    }
    atm_time_mono(&ts[1]);
    timediff = atm_time_mval(&ts[1], &ts[0]);
    printf("memcpy with cache line: %lu ms\n", timediff);
}


int main(int argc, char **argv)
{
    atm_sys_cache_line_profile();
    return 0;
}
