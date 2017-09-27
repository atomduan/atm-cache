#include <atm_linux_config.h>
#include <module_utils.h>

/**
 * *tmp++ = (i!=ssize-1) ? '1' : '\0';
 */

char *
native_bar(int argc, char **argv)
{
    int ssize=128, i;
    char **p = argv, *res, *tmp;

    printf("hello native...\n");
    for (i=0; *p!=NULL; ++i)
        printf("\targ%d: %s\n", i, *p++);

    res = malloc(ssize+1);
    for (i=0, tmp=res; i<ssize; ++i) *tmp++ = '1';

    res[ssize] = '\0';

    printf("module return res is :%s, %zu\n", res, strlen(res));
    return res;
}
