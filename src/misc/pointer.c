#include <atm_linux_config.h>
#include <misc_utils.h>

int main(int argc, char **argv) {
    char s[] = "adfasdfsdfasd";
    char *p;

    p = s;
    printf("kkkk : %s\n",&p[8]);
    p = s+10;
    printf("kkkk : %s\n",p);

    printf("s array addr %p\n", &s);
    printf("s array addr of first element %p\n", &s[0]);
}
