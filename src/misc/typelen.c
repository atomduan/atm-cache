#include <atm_linux_config.h>
#include <misc_utils.h>


int main(int argc, char **argv)
{
    printf("Time profile....\n");
    unsigned long int s;
    /**
     * sizeof returns size_t you need to use %zu for the format string instead of %d. 
     * The type of unsigned integer of size_t can vary (depending on platform) 
     * and may not be long unsigned int everywhere, 
     * which is covered in the draft C99 standard
     */
    printf("size of unsigned long int 's' is :%zu\n", sizeof(s));
    return 0;
}
