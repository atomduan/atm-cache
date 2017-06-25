#include <atm_core.h>

static void
atm_test_file_func(char *file);

/* -------------IMPL-------------- */
static void
atm_test_file_func(char *file)
{
    printf("file is : %s\n",file);
}


atm_uint_t
atm_test_file_suit()
{
    if (!ATM_TEST_FILE_ON) {
        atm_log("SKIP atm_test_file_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_file_suit");
    }

    if (!atm_test_file_traverse()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_file_traverse()
{
    atm_uint_t res = ATM_TEST_PASS;
    atm_file_traverse("/home/juntaoduan/"
            "Workspace/atm-cache/"
            "atm-cache/src",
             atm_test_file_func);
    return res;
}
