#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "interact.h"
#include "clp.h"
#include "signals.h"

#define BUF_SIZE 1024
#define MAX_ARGS 64

pid_t child_pid = -1;
volatile sig_atomic_t signal_received = 0;

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGQUIT, handle_sigquit);
    // signal(SIGTSTP, handle_sigtstp);
    
    // Handle zombie children
    signal(SIGCHLD, handle_sigchld);

    while (1) {
        // Display the prompt
        print_clp();

        // Read standard input and execute programs
        int exec = interact();

        if (signal_received == 1) {
            signal_received = 0;
            continue;
        }

        if (exec == -1) {
            // All error messages are showned in exec_func() in interact.c
            continue;
        }
        // Exit program
        else if (exec == -100) break;
    }

    return 0;
}
