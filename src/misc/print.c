#include <stdlib.h>
#include <stdio.h>

struct TS {
    struct TS *pre;
    struct TS *next;
    int data;
};

void initList(struct TS * ts, int a, int b)
{
    ts->pre = ts;
    ts->next = ts;
    ts->data = 1024 + a + b;
}

int main(int argc, char **argv)
{
    struct TS * list = malloc(sizeof(struct TS) * 10);
    int i=0;
    for (i = 0; i < 10; ++i) {
        initList(&list[i], 1, i);
        printf("current:0x%p, pre:0x%p, next:0x%p\n", &list[i], list[i].pre, list[i].next);
        printf("current:0x%p, pre:0x%p, next:0x%p, data=%d\n", &list[i], list[i].pre, list[i].next, list[i].data);
        printf("data=%d, current:%p, pre:%p, next:%p\n", list[i].data, &list[i], list[i].pre, list[i].next);
    }
    return 0;
}
