#include <atm_linux_config.h>
#include <misc_utils.h>


int main(int argc, char **argv)
{
    printf("Time profile....\n");
    struct timespec ts = {0,0};
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("CLOCK_REALTIME:%ld,%ld\n", ts.tv_sec, ts.tv_nsec);

    clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    printf("CLOCK_REALTIME_COARSE:%ld,%ld\n", ts.tv_sec, ts.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("CLOCK_MONOTONIC:%ld,%ld\n", ts.tv_sec, ts.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
    printf("CLOCK_MONOTONIC_COARSE:%ld,%ld\n", ts.tv_sec, ts.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    printf("CLOCK_MONOTONIC_RAW:%ld,%ld\n", ts.tv_sec, ts.tv_nsec);

    return 0;
}
