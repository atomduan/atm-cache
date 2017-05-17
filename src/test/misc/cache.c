#include <atm_linux_config.h>

/* the cpu cache line can 
 * heavyly impact the mm copy profermence
 * more than 100 times difference
 * SO, to zero-copy as possible as we can.
 */
#define DUMBCOPY for(int i=0; i<65536; i++) dest[i] = src[i]
#define SMARTCOPY memcpy(dest, src, 65536)
static void
atm_sys_cache_line_profile()
{
    printf("atm_sys_cache_line_profile......\n");
    char src[65536], dest[65536];
    for (int j = 0; j<10000; ++j) {
        DUMBCOPY;/* uncomment it and run */
        //SMARTCOPY;/* uncomment it and run */
    }
}


int main(int argc, char **argv)
{
    atm_sys_cache_line_profile();
    return 0;
}
