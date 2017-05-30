#ifndef _ATM_SESSION_H_INCLUDED_
#define _ATM_SESSION_H_INCLUDED_

#include <atm_core.h>


struct atm_sess_s {
    atm_conn_t  *conn;
    atm_int_t    argc;
    atm_str_t   *argv;
};


atm_sess_t *
atm_sess_new(atm_conn_t *conn);

void
atm_sess_free(void *se);

/* for every session (conn),
 * this func is in a single thread ctx,
 * no cocurrency
 * no session free harzard.
 * feel safe and free for biz impl in it
 * good bye multi-thread!
 *
 * so our resource has two level:
 * session level: is thread safe in this func
 * application level: have race condition between
 * different sessions*/
atm_int_t
atm_sess_process(atm_sess_t *se);


#endif /* _ATM_SESSION_H_INCLUDED_ */
