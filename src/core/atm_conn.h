#ifndef _ATM_CONN_H_INCLUDED_
#define _ATM_CONN_H_INCLUDED_

#include <atm_core.h>


struct atm_conn_s {
    void               *data;
    atm_event_t        *read;
    atm_event_t        *write;
    int                 fd;
};


void
atm_conn_init();


#endif /* _ATM_CONN_H_INCLUDED_ */
