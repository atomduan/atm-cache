#include <atm_linux_config.h>
#include <misc_utils.h>

struct foo {
    char  a;
    char  b;
    char  c;
    char  e;
    char  d;
    char  f;
    char  g;
    int name[10];
};

struct bar {
    char  a;
    int   l;
    char name[10];
};

static struct foo my_foo;
static struct bar my_bar;

int main(int argc, char **argv)
{
    printf("foo size is:%lu\n", sizeof(my_foo));
    printf("bar size is:%lu\n", sizeof(my_bar));
    char p = my_foo.a;
    char q = my_bar.a;
    printf("char is:%d\n", p);
    printf("char is:%d\n", q);
    return 0;
}
