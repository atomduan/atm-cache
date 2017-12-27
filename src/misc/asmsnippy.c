#include <atm_linux_config.h>
#include <misc_utils.h>

int main(int argc, char **argv)
{
    int a=1;
    int b=5;
    int c;
    c = a+b;
    __asm(
	"movl	%edi, -20(%rbp)\n"
	"movq	%rsi, -32(%rbp)\n"
	"movl	$1, -4(%rbp)\n"
	"movl	$5, -8(%rbp)\n"
	"movl	-4(%rbp), %edx\n"
	"movl	-8(%rbp), %eax\n"
	"addl	%edx, %eax\n"
	"movl	%eax, -12(%rbp)\n"
	"movl	-12(%rbp), %eax\n"
            );
    return c;
}
