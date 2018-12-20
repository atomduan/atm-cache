#include <atm_linux_config.h>
#include <misc_utils.h>

int main(int argc, char **argv)
{
    int a=1;
    int b=5;
    int c;
    c = a+b;
    __asm__ (
	"    mov    %ebx, %esi;  "
            );
    return 0;
}
