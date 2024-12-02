#ifndef __CLP__
#define __CLP__

#define BUF_SIZE 1024
#define MAX_ARGS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

char* get_username();
char* get_hostname();
char* get_pwd();

// Print command-line prompt
void print_clp();

#endif //__CLP__

