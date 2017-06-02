#include <atm_core.h>


/* -------------IMPL-------------- */


atm_uint_t
atm_test_str_suit()
{
    if (!ATM_TEST_STR_ON) {
        atm_log("SKIP atm_test_str_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_str_suit");
    }

    if (!atm_test_str_split()) {
        return ATM_TEST_FAIL;
    }

    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_str_split()
{
    atm_log("atm_test_str_split");
    char *a;
    atm_str_t *b;

    a = " a    b  d s \"   'a'   \"   kkk  k";
    atm_log("strim org is: %s", a);
    b = atm_str_split(a,' ',strlen(a));
    for (;b!=NULL && *b!=NULL; b++) {
        atm_log("strim res len: %d, val: %s", atm_str_len(*b), *b);
    }

    a = "a";
    atm_log("strim org is: %s", a);
    b = atm_str_split(a,' ',strlen(a));
    for (;b!=NULL && *b!=NULL; b++) {
        atm_log("strim res len: %d, val: %s", atm_str_len(*b), *b);
    }

    a = "         ";
    atm_log("strim org is: %s", a);
    b = atm_str_split(a,' ',strlen(a));
    for (;b!=NULL && *b!=NULL; b++) {
        atm_log("strim res len: %d, val: %s", atm_str_len(*b), *b);
    }

    a = "";
    atm_log("strim org is: %s", a);
    b = atm_str_split(a,' ',strlen(a));
    for (;b!=NULL && *b!=NULL; b++) {
        atm_log("strim res len: %d, val: %s", atm_str_len(*b), *b);
    }

    return ATM_TEST_PASS;
}
