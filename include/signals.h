#ifndef __SIGNALS__
#define __SIGNALS__

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
extern volatile sig_atomic_t signal_received;

void handle_sigint(int dummy);
void handle_sigterm(int dummy);
void handle_sigquit(int dummy);
void handle_sigtstp(int dummy);

void handle_sigchld(int dummy);

#endif //__SIGNALS__

