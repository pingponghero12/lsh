#ifndef __EXECS__
#define __EXECS__

#define BUF_SIZE 1024
#define MAX_ARGS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "interact.h"

extern pid_t child_pid;
extern volatile sig_atomic_t signal_received;

int exec_func(char **args, int *background);
int exec_pipeline(char ***cmds, int num_cmds, int background);

#endif //__EXECS__

