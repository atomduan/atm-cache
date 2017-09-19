#include <atm_linux_config.h>
#include <module_utils.h>

void *
native_bar(int argc, char **argv, char **res)
{
    int i = 0;
    int j = 0;
    char **p = argv;
    char *tmp;

    for (; *p!=NULL; j++) {
        printf("hello native...%d, %s\n", j, *p++);
    }

    *res = malloc(10);

    tmp = *res;
    for (i=0; i<10; i++) {
        *tmp++ = (i!=9) ? '1' : '\0';
    }

    printf("res is %s\n", *res);
    return res;
}
