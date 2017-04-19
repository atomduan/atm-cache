#include <atm_core.h>

static const atm_str_t ATM_LOG_LV_ENUM[] = {
    "OFF",
    "ALL",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};


static atm_str_t atm_log_lvstr(atm_uint_t level) 
{
    atm_uint_t lvs_size = sizeof(ATM_LOG_LV_ENUM)/sizeof(atm_str_t);
    if (level >= lvs_size)
        level = lvs_size - 1;
    return ATM_LOG_LV_ENUM[level];
}


static void atm_log_routine_raw(atm_uint_t level, atm_str_t msg) 
{
    printf("[%s]:%s\n", atm_log_lvstr(level), msg);
}


static void atm_log_snprintf(
        atm_str_t msg, atm_int_t size, atm_str_t fmt, va_list args) 
{
    vsnprintf(msg, size, fmt, args); 
}


void atm_log(atm_str_t fmt, ...) 
{
    atm_str_t message = (atm_str_t)atm_malloc(ATM_LOG_MAX_LEN);
    va_list args;
    va_start(args, fmt);
    atm_log_snprintf(message, ATM_LOG_MAX_LEN, fmt, args);
    va_end(args);
    atm_log_routine_raw(ATM_LOG_INFO, message);
    atm_free(message);
}


void atm_log_routine(atm_uint_t level, atm_str_t fmt, ...) 
{
    atm_str_t message 
        = (atm_str_t)atm_malloc(ATM_LOG_MAX_LEN);
    va_list args;
    va_start(args, fmt);
    atm_log_snprintf(message, ATM_LOG_MAX_LEN, fmt, args);
    va_end(args);
    atm_log_routine_raw(level, message);
    atm_free(message);
}
