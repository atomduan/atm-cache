#ifndef _ATM_LOGGER_H_INCLUDED_
#define _ATM_LOGGER_H_INCLUDED_

#include <atm_config.h>
#include <atm_core.h>

#define ATM_LOG_OFF     0
#define ATM_LOG_ALL     1
#define ATM_LOG_DEBUG   2
#define ATM_LOG_INFO    3
#define ATM_LOG_WARN    4
#define ATM_LOG_ERROR   5
#define ATM_LOG_FATAL   6

#define ATM_LOG_MAX_LEN   1024

atm_str_t atm_log_lvstr(atm_uint_t level);

void atm_log_snprintf(
        atm_str_t msg, atm_int_t size, atm_str_t fmt, va_list args);

void atm_log(atm_str_t fmt, ...);

void atm_log_routine(atm_uint_t level, atm_str_t fmt, ...);

void atm_log_routine_raw(atm_uint_t level, atm_str_t msg);

#endif
