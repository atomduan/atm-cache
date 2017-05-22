#include <atm_linux_config.h>
#include <misc_utils.h>


int main(int argc, char **argv)
{
    void *old_ptr = NULL;
    void *new_ptr = NULL;
    if (old_ptr == new_ptr) {
        printf("two NULL ptrs can be tested to equals\n");
    } else {
        printf("two NULL ptrs can NOT be tested to equals\n");
    }
    return 0;
}
