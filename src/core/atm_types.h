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
 * Byte oriented data type define:
 *      we recommand to use unint*_t 
 *      to represent bit wise data type 
 */
typedef uint8_t         atm_byte_t;/* same to u_char */


/*
 * Char and string relevant
 */
typedef struct atm_str_s atm_str_t;
struct atm_str_s {
    char       *val;
    atm_int_t   len;
};


/*
 * Non Primitive Data types define
 */
/* General Type For Container New */
typedef struct atm_T_s              atm_T_t;
struct atm_T_s {
    void              *spec;
    atm_bool_t      (* match)(void *v1, void *v2);
    uint64_t        (* hash)(void *v);
    atm_int_t       (* compare)(void *v1, void *v2);
    atm_str_t      *(* str)(void *v);
    void            (* free)(void *v);
};


/* Specific Types */
typedef struct atm_list_s           atm_list_t;
typedef struct atm_dict_s           atm_dict_t;


#endif /* _ATM_TYPES_H_INCLUDED_ */
