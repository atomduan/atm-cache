#include <atm_linux_config.h>
#include <module_utils.h>

void
native_bar()
{
    int i = 1000;
    while (--i > 0) {
        usleep(1000*1000);
        printf("hello native...%d\n", i);
    }
}
