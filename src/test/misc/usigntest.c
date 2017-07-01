#include <atm_linux_config.h>
#include <misc_utils.h>

int main(int argc, char **argv)
{
    unsigned int a = 4294967295;
    a = 1245;
    int b = 123;
    //a = b;
    b = a;
    printf("r is %u\n",b);
    printf("r is %d\n",b);
    return 0;
}
