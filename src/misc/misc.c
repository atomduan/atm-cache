#include <atm_linux_config.h>
#include <misc_utils.h>

static void
atm_sys_profile()
{
   int pgsz = getpagesize();
   printf("Memory page size is %d Byte\n", pgsz);
}


int main(int argc, char **argv)
{
    char * p = NULL;
    printf("p addr is %p\n", p);
    atm_sys_profile();
    printf("TEST_BAR macro is %d\n", TEST_BAR);
    printf("RES_NANO_SEC is %d\n", RES_NANO_SEC);
    return 0;
}
