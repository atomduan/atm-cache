#include <atm_linux_config.h>

#define RES_NANO_SEC    1000000000
#define RES_MICR_SEC    1000000
#define RES_MILI_SEC    1000

#define TEST_FOO        100
#define TEST_BAR        TEST_FOO


typedef struct timespec    atm_timespec_t;


unsigned long
atm_time_mval(atm_timespec_t *rct,
        atm_timespec_t *old);

void
atm_time_mono(atm_timespec_t *ts);
