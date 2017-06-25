#ifndef _ATM_FILE_H_INCLUDED_
#define _ATM_FILE_H_INCLUDED_

#include <atm_core.h>

#define ATM_FILE_UNKNOWN    0
#define ATM_FILE_REG        1
#define ATM_FILE_DIR        2


atm_int_t
atm_file_nonblock(int fd, atm_bool_t nblk);

atm_str_t
atm_file_exe_path();

atm_str_t
atm_file_find(char *dir, char *file_name, int type);

atm_str_t
atm_file_path_append(char *parent, char *file);


#endif /* _ATM_FILE_H_INCLUDED_ */
