#include <atm_linux_config.h>

static void
atm_sys_profile()
{
   int pgsz = getpagesize(); 
   printf("Memory page size is %d Byte\n", pgsz);
}


int main(int argc, char **argv)
{
    atm_sys_profile();
    return 0;
}
