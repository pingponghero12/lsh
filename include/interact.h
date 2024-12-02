#ifndef __INTERACT__
#define __INTERACT__

#define BUF_SIZE 1024
#define MAX_ARGS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern pid_t child_pid;

int interact();
int read_input(char **args);
int exec_func(char **args);

#endif //__INTERACT__

