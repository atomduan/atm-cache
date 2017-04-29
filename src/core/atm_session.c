#include <atm_core.h>


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
atm_session_t *
atm_session_new(atm_conn_t *conn)
{
    atm_session_t *res = NULL;

    res = atm_alloc(sizeof(atm_session_t)); 
    res->r_buf = atm_buf_new();
    res->w_buf = atm_buf_new();
    return res;
}


void
atm_session_process(atm_session_t *se)
{

}
