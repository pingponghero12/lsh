#ifndef __INTERACT__
#define __INTERACT__

#define BUF_SIZE 1024
#define MAX_ARGS 64
#define MAX_CMDS 10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "execs.h"

extern pid_t child_pid;
extern volatile sig_atomic_t signal_received;

int interact();
int read_input(char ***cmds, int *num_cmds, int *background);

#endif //__INTERACT__

