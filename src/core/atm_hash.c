#include <atm_core.h>
/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
void 
atm_hash_init()
{
    atm_siphash_init();
}


uint64_t 
atm_hash(char *input, atm_uint_t inlen)
{
    return atm_siphash(input, inlen);
}


uint64_t 
atm_hash_nocase(char *input, atm_uint_t inlen)
{
    return atm_siphash_nocase(input, inlen);
}
