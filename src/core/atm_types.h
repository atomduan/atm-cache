#ifndef _ATM_TYPES_H_INCLUDED_
#define _ATM_TYPES_H_INCLUDED_
/*
 * MUST BE INCLUDED BEFORE ANY OTHERS BIZ HEADERS
 */
/* 
 * Primitive data type define
 */
typedef int8_t          atm_bool_t;

/* int64_t or int32_t */
typedef int32_t         atm_int_t; 
/* In 64 system same to: size_t and unsigned long int
 * In 32 system: same to: size_t and unsigned int 
 */
typedef uint32_t        atm_uint_t;
typedef int64_t         atm_long_t;
typedef uint64_t        atm_ulong_t;

/*
 * Char and string relevant
 */
typedef char            atm_char_t;
typedef atm_char_t *    atm_str_t;

/* 
 * Byte oriented data type define:
 *      we recommand to use unint*_t 
 *      to represent bit wise data type 
 */
typedef uint8_t         atm_byte_t;/* same to u_char */

/*
 * Biz system headers.
 */
typedef struct atm_list_T_s         atm_list_T_t;
typedef struct atm_list_ele_s       atm_list_ele_t;
typedef struct atm_list_s           atm_list_t;

typedef struct atm_dict_T_s         atm_dict_T_t;
typedef struct atm_dict_entry_s     atm_dict_entry_t;
typedef struct atm_dict_s           atm_dict_t;


#endif /* _ATM_TYPES_H_INCLUDED_ */