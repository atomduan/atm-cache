#include <atm_config.h>
#include <atm_core.h>


atm_bool_t atm_string_equal(
        const atm_string_t *s1, const atm_string_t *s2) {
    return strcmp(s1->str, s2->str)==0 ? ATM_TRUE : ATM_FALSE;
}

void atm_string_free(atm_string_t *s) {
   atm_free(s->str);
   atm_free(s); 
}
