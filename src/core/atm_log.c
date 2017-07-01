#include <atm_core.h>
/*
 * Private
 * */
static const char *
atm_log_lvstr(atm_uint_t level);
static void
atm_log_rout_raw(atm_uint_t lv, char *msg);


static const char * ATM_LOG_LV_ENUM[] = {
    "OFF",
    "ALL",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static const char *
atm_log_lvstr(atm_uint_t level)
{
    atm_uint_t lvs_size = 0;
    lvs_size = sizeof(ATM_LOG_LV_ENUM)/sizeof(char *);
    if (level >= lvs_size)
        level = lvs_size - 1;
    return ATM_LOG_LV_ENUM[level];
}


static void
atm_log_rout_raw(atm_uint_t lv, char *msg)
{
    atm_uint_t llv;
    atm_str_t now;
    
    llv = atm_config->loglevel;
    now = atm_time_strftime_now();
    if (msg != NULL) {
        if (llv && lv) {
            if (llv <= lv) {
                printf("[%s] %s : %s\n",atm_log_lvstr(lv),now,msg);
            }
        }
    }
    atm_str_free(now);
}


int
atm_log_parse_level(char *lv)
{
    int res = -1;
    int size;
    int i;

    atm_str_t slv = atm_str_new(lv);
    if (slv != NULL) {
        atm_str_toupper(slv);
        size = sizeof(ATM_LOG_LV_ENUM)/sizeof(char *);
        for (i=0; i<size; i++) {
            if (!strcmp(slv,ATM_LOG_LV_ENUM[i])) {
                res = i; 
                break;
            }
        }
    }
    return res;
}


void
atm_log(char *fmt, ...)
{
    atm_uint_t size = ATM_LOG_MAXLEN;
    char msg[size];
    memset(msg,ATM_MEM_ZERO,size);

    va_list args;
    va_start(args,fmt);
    vsnprintf(msg,size,fmt,args);
    va_end(args);

    atm_log_rout_raw(ATM_LOG_INFO,msg);
}


void
atm_log_err(char *fmt, ...)
{
    atm_uint_t size = ATM_LOG_MAXLEN;
    atm_str_t now;
    char msg[size];
    memset(msg,ATM_MEM_ZERO,size);
    
    va_list args;
    va_start(args,fmt);
    vsnprintf(msg,size,fmt,args);
    va_end(args);

    now = atm_time_strftime_now();
    if (msg != NULL) {
        fprintf(stderr,"%s : %s\n",now,msg);
        atm_log_fflush();
    }
    atm_str_free(now);
}


void
atm_log_rout(atm_uint_t level, char *fmt, ...)
{
    atm_uint_t size = ATM_LOG_MAXLEN;
    char msg[size];
    memset(msg,ATM_MEM_ZERO,size);

    va_list args;
    va_start(args,fmt);
    vsnprintf(msg,size,fmt,args);
    va_end(args);

    atm_log_rout_raw(level, msg);
}


void
atm_log_fflush()
{
    /* TODO SHOULD FLUSH ALL REGULAR FILE */
    fflush(stderr);
    fflush(stdout);
}
