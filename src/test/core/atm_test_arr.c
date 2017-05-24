#include <atm_core.h>

/* -------------IMPL-------------- */
atm_uint_t
atm_test_arr_suit()
{
    if (!ATM_TEST_ARR_ON) {
        atm_log("SKIP atm_test_arr_suit");
        return ATM_TEST_PASS;
    } else {
        atm_log("BEGIN atm_test_arr_suit");
    }

    //if (!atm_test_arr_get()) {
    //    return ATM_TEST_FAIL;
    //}
    if (!atm_test_arr_del()) {
        return ATM_TEST_FAIL;
    }
    return ATM_TEST_PASS;
}


atm_uint_t
atm_test_arr_get()
{
    atm_uint_t size = 100;
    atm_uint_t res = ATM_TEST_PASS;
    atm_arr_t *a = atm_arr_new(sizeof(atm_uint_t));
    atm_uint_t i;
    for (i=0; i<size; i++) {
        atm_uint_t t = i;
        atm_arr_add(a,&t);
        atm_log("------------------");
        atm_log("arr cap is %u", a->capacity);
        atm_log("arr len is %u", a->length);
    }
    for (i=0; i<a->length; i++) {
        atm_log("------------------");
        atm_log("arr val is [%u,%u]", i, *(atm_uint_t *)atm_arr_get(a,i));
    }
    for (i=0; i<a->length; i++) {
        atm_uint_t t = a->length - i;
        atm_arr_set(a,i,&t);
    }
    for (i=0; i<a->length; i++) {
        atm_log("------------------");
        atm_log("arr val is [%u,%u]", i, *(atm_uint_t *)atm_arr_get(a,i));
    }
    atm_test_report("atm_test_arr_get", res);
    return res;
}


atm_uint_t
atm_test_arr_del()
{
    atm_uint_t size = 100;
    atm_uint_t res = ATM_TEST_PASS;
    atm_arr_t *a = atm_arr_new(sizeof(atm_uint_t *));
    atm_uint_t i;
    for (i=0; i<size; i++) {
        atm_uint_t t = i;
        atm_arr_add(a,&t);
    }
    for (i=0; i<a->length; i++) {
        atm_uint_t t = a->length - i;
        atm_arr_set(a,i,&t);
    }
    for (i=a->length; i>0; i-=2) {
        atm_arr_del(a,i);
        atm_log("arr cap is %u", a->capacity);
        atm_log("arr len is %u", a->length);
    }
    for (i=0; i<a->length; i++) {
        atm_log("------------------");
        atm_log("arr val is [%u,%u]", i, *(atm_uint_t *)atm_arr_get(a,i));
    }

    while (atm_arr_del(a,0) == ATM_OK) {
        atm_log("------------------");
        atm_log("arr cap is %u", a->capacity);
        atm_log("arr len is %u", a->length);
    }
    atm_log("------------------");
    atm_log("arr cap is %u", a->capacity);
    atm_log("arr len is %u", a->length);

    atm_test_report("atm_test_arr_del", res);
    return res;
}
