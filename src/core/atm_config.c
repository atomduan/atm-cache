#include <atm_core.h>
/*
 * Private
 * */
static atm_bool_t
atm_config_yesnotoi(char *s);
static void
atm_config_process(atm_str_t conf_str);
static atm_str_t
atm_config_path();
static void
atm_config_load();


static atm_config_t *config;


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static atm_bool_t
atm_config_yesnotoi(char *s)
{
    if(!strcasecmp(s,"yes")) return 1; 
    else 
    if(!strcasecmp(s,"no")) return 0;
    else return -1;
}


static void
atm_config_process(atm_str_t conf_str)
{
    atm_str_t cs = conf_str;
    atm_str_t *lines;
    atm_str_t *ls;
    atm_str_t l;
    int conf_len, line_num; 

    atm_str_t errmsg;
    atm_str_t *props;
    atm_str_t pname;
    atm_int_t psize;

    conf_len = atm_str_len(conf_str);
    lines = atm_str_split(cs,'\n',conf_len);

    if (lines == NULL) return;
    for (ls=lines,line_num=0; *ls!=NULL; ls++,line_num++) {
        l = atm_str_trim(*ls," \t\r\n");
        if (l[0]=='#'||l[0]=='\0') continue;
        props = atm_str_split(l,' ',atm_str_len(l));
        psize = atm_str_split_size(props);
        pname = atm_str_tolower(props[0]);

        if (atm_str_eqs(pname,"pidfile") && psize==2) {
            config->pidfile = atm_str_dup(props[1]);
        } else
        if (atm_str_eqs(pname,"daemonize") && psize==2) {
            config->daemonize = atm_config_yesnotoi(props[1]);
            if (config->daemonize == -1) {
                errmsg = "arguments must yes or no"; goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"bind")) {
            int j, addresses = psize-1;
            if (addresses > ATM_CONFIG_MAXBINDADDR) {
                errmsg = "Too many bind addresses specified"; goto loaderr;
            }
            config->bindaddr = atm_alloc(
                    sizeof(atm_str_t *)*ATM_CONFIG_MAXBINDADDR + 1);
            for (j = 0; j < addresses; j++)
                config->bindaddr[j] = atm_str_dup(props[j+1]);
            config->bindaddr_count = addresses;
        } else
        if (atm_str_eqs(pname,"port") && psize==2) {
            config->port = atoi(props[1]);
            if (config->port < 0 || config->port > 65535) {
                errmsg = "Invalid port"; goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"tcpbacklog") && psize==2) {
            config->tcpbacklog = atoi(props[1]);
            if (config->tcpbacklog < 0) {
                errmsg = "Invalid tcpbacklog value"; goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"timeout") && psize==2) {
            config->maxidletime = atoi(props[1]);
            if (config->maxidletime < 0) {
                errmsg = "Invalid timeout value"; goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"tcpkeepalive") && psize==2) {
            config->tcpkeepalive = atoi(props[1]);
            if (config->tcpkeepalive < 0) {
                errmsg = "Invalid tcp-keepalive value"; goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"maxclients") && psize==2) {
            config->maxclients = atoi(props[1]);
            if (config->maxclients < 1) {
                errmsg = "Invalid max clients limit"; goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"loglevel") && psize==2) {
            config->loglevel = atoi(props[1]);
            if (config->loglevel < 0 || config->loglevel > ATM_LOG_FATAL) {
                errmsg = "Invalid log level. "
                      "Must be one of debug, verbose, notice, warning";
                goto loaderr;
            }
        } else
        if (atm_str_eqs(pname,"maxmemory") && psize==2) {
            config->maxmemory = atm_util_memtoll(props[1],NULL);
        } else
        if (atm_str_eqs(pname,"workernum") && psize==2) {
            config->workernum = atoi(props[1]);
            if (config->workernum < 1) {
                errmsg = "Invalid max clients limit"; goto loaderr;
            }
        }

        atm_str_split_free(props);
    }
    atm_str_split_free(lines);
    return;

loaderr:
    atm_log_rout(ATM_LOG_FATAL,
            "\n*** FATAL CONFIG FILE ERROR ***\n"
            "Reading the configuration file, at line %d\n"
            ">>> '%s'\n"
            "%s\n",line_num,lines[line_num],errmsg);
    exit(ATM_ERROR);
}


static atm_str_t
atm_config_path()
{
    atm_str_t res = NULL;
    res = atm_str_new("/home/juntaoduan/Workspace"
            "/atm-cache/atm-cache"
            "/config/atmcache.conf");
    return res;
}


static void
atm_config_load()
{
    atm_uint_t bsz = ATM_CONFIG_MAXLINE+1;
    char buf[bsz];
    atm_str_t conf;
    atm_str_t conf_path;

    conf = atm_str_new("");
    conf_path = atm_config_path();

    if (conf_path != NULL) {
        FILE *fp;
        if((fp = fopen(conf_path,"r"))==NULL) {
            atm_log_rout(ATM_LOG_FATAL,
                "can not open config file[%s]",conf_path);
            exit(ATM_ERROR);
        }
        while(fgets(buf,bsz,fp) != NULL) {
            conf = atm_str_cats(conf,buf);
        }
        fclose(fp);
        config->configfile = conf_path;
    }
    atm_config_process(conf);
    atm_str_free(conf);
}


/*
 * Public
 * */
void
atm_config_init()
{
    config = atm_alloc(sizeof(atm_config_t));
    atm_ctx->config = config;
    atm_config_load();
}
