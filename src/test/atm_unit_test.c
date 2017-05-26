#include <atm_core.h>
/*
 * Private
 * */
static void
atm_test_chd_exit_eval(int stat);

static int
atm_test_suit_proc(int argc, char **argv);


/* ---------------------IMPLEMENTATIONS--------------------------- */
/*
 * Private
 * */
static void
atm_test_chd_exit_eval(int stat)
{
    if (WIFEXITED(stat)) {
        atm_log("test chd exit normally with code %d",
                WEXITSTATUS(stat));
    }
    if (WIFSIGNALED(stat)) {
        int sig = WTERMSIG(stat);
        atm_log("test chd terminate by signal %s",
                strsignal(sig));
    }
}


static atm_int_t
atm_test_suit_proc(int argc, char **argv)
{
    atm_log("enter ");
    atm_test_siphash_suit();
    atm_test_dict_suit();
    atm_test_event_suit();
    atm_test_str_suit();
    atm_test_time_suit();
    atm_test_arr_suit();
    return ATM_OK;
}


/*
 * Public
 * */
void
atm_test_suit(int argc, char **argv)
{
    int stat = 0;
    pid_t pid = 0, wpid;

    atm_log("atm_test_suit entry");
    if ((pid = fork()) < 0) {
        atm_log("fork error occure");
        exit(ATM_FATAL);
    } else if (pid == 0) {
        atm_test_suit_proc(argc, argv);
        exit(ATM_OK);
    }

    wpid = wait(&stat);
    if(wpid == -1) {
        atm_log("test wait chd error");
        exit(1);
    }
    atm_test_chd_exit_eval(stat);

    atm_log("test main complete");
}


void
atm_test_report(char *func_name, atm_uint_t res)
{
    if (ATM_TEST_PASS == res) {
        atm_log("PASS : test func name is [%s]",
                func_name);
    }
    if (ATM_TEST_FAIL == res) {
        atm_log("FAIL : test func name is [%s]",
                func_name);
    }
}
