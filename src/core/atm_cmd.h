#ifndef _ATM_CMD_H_INCLUDED_
#define _ATM_CMD_H_INCLUDED_

#include <atm_core.h>


struct atm_cmd_s {
    atm_str_t    name;
    atm_int_t  (*func)(atm_sess_t *se);
    atm_int_t    arity;
};


/* public buf type lifecycle */
void
atm_cmd_init();

atm_cmd_t *
atm_cmd_getcmd(atm_str_t name);

#endif /* _ATM_CMD_H_INCLUDED_ */
