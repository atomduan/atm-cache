#include <atm_core.h>


static void 
atm_log_rout_raw(atm_uint_t lv, atm_str_t *msg);


static const char * ATM_LOG_LV_ENUM[] = {
    "OFF",
    "ALL",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};


static const char *
atm_log_lvstr(atm_uint_t level) 
{
    atm_uint_t lvs_size = 0;

    lvs_size = 
        sizeof(ATM_LOG_LV_ENUM)/sizeof(char *);

    if (level >= lvs_size)
        level = lvs_size - 1;
    return ATM_LOG_LV_ENUM[level];
}


static void 
atm_log_rout_raw(atm_uint_t lv, atm_str_t *msg) 
{
    if (msg == NULL && msg->val != NULL) {
        printf("[%s]:%s\n", 
            atm_log_lvstr(lv), msg->val);
    } else {
        printf("Invalid msg for logger");
    }
}


void 
atm_log(char *fmt, ...) 
{
    atm_str_t *msg = NULL;

    va_list args;
    va_start(args, fmt);
    msg = atm_str_vfmt(fmt, args);
    va_end(args);

    atm_log_rout_raw(ATM_LOG_INFO, msg);
    atm_free(msg);
}


void 
atm_log_rout(atm_uint_t level, char *fmt, ...) 
{
    atm_str_t *msg = NULL;

    va_list args;
    va_start(args, fmt);
    msg = atm_str_vfmt(fmt, args);
    va_end(args);

    atm_log_rout_raw(level, msg);
    atm_free(msg);
}
