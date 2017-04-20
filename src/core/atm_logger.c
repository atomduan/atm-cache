#include <atm_core.h>

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
atm_log_routine_raw(atm_uint_t level, char *msg) 
{
    printf("[%s]:%s\n", atm_log_lvstr(level), msg);
}


static void 
atm_log_snprintf(char *msg, atm_int_t size,
        char *fmt, va_list args) 
{
    vsnprintf(msg, size, fmt, args); 
}


void 
atm_log(char *fmt, ...) 
{
    char *msg = NULL;

    msg = (char *) atm_malloc(ATM_LOG_MAX_LEN);

    va_list args;
    va_start(args, fmt);
    atm_log_snprintf(msg, ATM_LOG_MAX_LEN, fmt, args);
    va_end(args);

    atm_log_routine_raw(ATM_LOG_INFO, msg);
    atm_free(msg);
}


void 
atm_log_routine(atm_uint_t level, char *fmt, ...) 
{
    char *msg = NULL;

    msg = (char *) atm_malloc(ATM_LOG_MAX_LEN);

    va_list args;
    va_start(args, fmt);
    atm_log_snprintf(msg, ATM_LOG_MAX_LEN, fmt, args);
    va_end(args);

    atm_log_routine_raw(level, msg);
    atm_free(msg);
}
