#ifndef _ATM_CORE_H_INCLUDED_
#define _ATM_CORE_H_INCLUDED_


/*
 * -------------- Basic System Config --------------
 */


#include <atm_auto_headers.h>

#if (ATM_LINUX)
#include <atm_linux_config.h>
#endif

#define ATM_OK          0
#define ATM_ERROR      -1
#define ATM_FATAL      -1

#define ATM_TRUE        1
#define ATM_FALSE       0

#define ATM_CMP_LT     -1
#define ATM_CMP_EQ      0
#define ATM_CMP_GT      1


/*
 * -------------- Types Definition --------------
 */


/*
 * Primitive data type define
 */
typedef int8_t              atm_bool_t;

/* int64_t or int32_t */
typedef int32_t             atm_int_t;

/* In 64 system same to: size_t and unsigned long int
 * In 32 system: same to: size_t and unsigned int
 */
typedef uint32_t            atm_uint_t;
typedef int64_t             atm_long_t;
typedef uint64_t            atm_ulong_t;

/*
 * Byte oriented data type define:
 *      we recommand to use unint*_t
 *      to represent bit wise data type
 */
typedef uint8_t             atm_byte_t;/* same to u_char */

/*
 * Char and string relevant
 */
typedef char               *atm_str_t;

/*
 * Non Primitive Data types define
 */
/* General Type For Container New */
typedef struct atm_T_s atm_T_t;
struct atm_T_s {
    void              *spec;
    atm_bool_t      (* match)(void *v1, void *v2);
    uint64_t        (* hash)(void *v);
    atm_int_t       (* compare)(void *v1, void *v2);
    atm_str_t       (* str)(void *v);
    void            (* free)(void *v);
};

/* Specific Free Types */
#define ATM_FREE_SHALLOW    0
#define ATM_FREE_DEEP       1

/* Specific Container Types */
typedef struct atm_list_s           atm_list_t;
typedef struct atm_list_iter_s      atm_list_iter_t;
typedef struct atm_dict_s           atm_dict_t;
typedef struct atm_conn_s           atm_conn_t;
typedef struct atm_socket_s         atm_socket_t;
typedef struct atm_event_s          atm_event_t;
typedef struct atm_task_s           atm_task_t;
typedef struct atm_sess_s           atm_sess_t;
typedef struct atm_buf_s            atm_buf_t;
typedef struct atm_blk_s            atm_blk_t;
typedef struct atm_ctx_s            atm_ctx_t;
typedef struct atm_arr_s            atm_arr_t;
typedef struct timespec             atm_time_spec_t;
typedef struct atm_time_pair_s      atm_time_pair_t;
typedef struct atm_pipe_s           atm_pipe_t;
typedef struct atm_queue_s          atm_queue_t;
typedef struct atm_cmd_s            atm_cmd_t;
typedef struct atm_config_s         atm_config_t;
typedef struct atm_obj_s            atm_obj_t;


/*
 * Function type define
 */
typedef void 
(*atm_file_callback)(char *,void *) ;
typedef void 
(*atm_ctx_callback)() ;


/*
 * -------------- Dep headers --------------
 */
#include <atm_util.h>

#include <atm_alloc.h>
#include <atm_atomic.h>
#include <atm_file.h>
#include <atm_pipe.h>
#include <atm_net.h>
#include <atm_sig.h>

#include <atm_arr.h>
#include <atm_blk.h>
#include <atm_buf.h>
#include <atm_cmd.h>
#include <atm_config.h>
#include <atm_conn.h>
#include <atm_ctx.h>
#include <atm_dict.h>
#include <atm_event.h>
#include <atm_list.h>
#include <atm_log.h>
#include <atm_obj.h>
#include <atm_queue.h>
#include <atm_sess.h>
#include <atm_siphash.h>
#include <atm_str.h>
#include <atm_task.h>
#include <atm_time.h>

#ifdef ATM_UNIT_TEST
#include <atm_unit_test.h>
#endif


#endif /* _ATM_CORE_H_INCLUDED_ */
