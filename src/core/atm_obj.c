#include <atm_core.h>
/*
 * Private
 * */
static atm_T_t ATM_OBJ_TYPE = {
    NULL,
    atm_obj_match,
    atm_obj_hash,
    atm_obj_compare,
    atm_obj_str,
    atm_obj_free,
};


/*
 * Public
 * */
atm_T_t *ATM_OBJ_T = &ATM_OBJ_TYPE;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Public
 * */
atm_obj_t *
atm_obj_new(atm_T_t *type, void *v)
{
    atm_obj_t *res = NULL;
    res = atm_alloc(sizeof(atm_obj_t));
    res->v_type = type;
    res->value = v;
    return res;
}


atm_bool_t
atm_obj_match(void *v1, void *v2)
{
    atm_obj_t *o1 = v1;
    atm_obj_t *o2 = v2;
    if (!o1 && !o2) return ATM_TRUE;
    if ((o1 && !o2) || (o2 && !o1)) return ATM_FALSE;
    if (o1->v_type != o2->v_type) return ATM_FALSE;
    if (o1->value == o2->value) return ATM_TRUE;
    if (o1->v_type && o2->v_type) {
        if(o1->v_type->match) {
            void *val1 = o1->value;
            void *val2 = o2->value;
            return o1->v_type->match(val1,val2);
        }
    }
    return ATM_FALSE;
}


uint64_t
atm_obj_hash(void *v)
{
    uint64_t res = 0;
    atm_obj_t *o = v;
    if (o && o->v_type && o->v_type->hash){
        res = o->v_type->hash(o->value);
    }
    return res;
}


atm_int_t
atm_obj_compare(void *v1, void *v2)
{
    atm_int_t res = 0;
    atm_obj_t *o1 = v1;
    atm_obj_t *o2 = v2;
    if (o1 && o2
            && o1->v_type && o2->v_type
            && o1->v_type == o2->v_type) {
        void *val1 = o1->value;
        void *val2 = o2->value;
        res = o1->v_type->compare(val1,val2);
    }
    return res;
}


atm_str_t
atm_obj_str(void *v)
{
    atm_str_t res = NULL;
    atm_obj_t *o = v;
    if (o && o->v_type && o->v_type->str){
        res = o->v_type->str(o->value);
    }
    return res;
}


void
atm_obj_free(void *v)
{
    atm_obj_t *o = v;
    if (o != NULL) {
        if (o->v_type && o->v_type->free){
            o->v_type->free(o->value);
        }
        atm_free(o);
    }
}
