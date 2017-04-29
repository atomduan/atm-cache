#ifndef _ATM_SESSION_H_INCLUDED_
#define _ATM_SESSION_H_INCLUDED_

#include <atm_core.h>


struct atm_session_s {
    atm_buf_t   *r_buf;
    atm_buf_t   *w_buf;
};


atm_session_t *
atm_session_new(atm_conn_t *conn);

void
atm_session_free(void *se);

void
atm_session_process(atm_session_t *se);


#endif /* _ATM_SESSION_H_INCLUDED_ */
