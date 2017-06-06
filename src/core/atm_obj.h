#ifndef _ATM_OBJ_H_INCLUDED_
#define _ATM_OBJ_H_INCLUDED_


extern atm_T_t *ATM_OBJ_T;

struct atm_obj_s {
    atm_T_t    *v_type;
    void       *value;
};


/* public string type lifecycle */
atm_obj_t *
atm_obj_new(atm_T_t *type, void *v);

atm_bool_t
atm_obj_match(void *v1, void *v2);

uint64_t
atm_obj_hash(void *v);

atm_int_t
atm_obj_compare(void *v1, void *v2);

atm_str_t
atm_obj_str(void *v);

void
atm_obj_free(void *v);

#endif /* _ATM_OBJ_H_INCLUDED_ */
