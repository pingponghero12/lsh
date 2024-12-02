#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "interact.h"
#include "clp.h"

#define BUF_SIZE 1024
#define MAX_ARGS 64

pid_t child_pid = -1;
volatile sig_atomic_t sigint_received = 0;

void handle_sigint(int dummy) {
    sigint_received = 1;

    if (child_pid > 0) {
        kill(child_pid, SIGINT);
        waitpid(child_pid, NULL, WNOHANG);
    }

    const char skipline[] = "\n";
    write(STDOUT_FILENO, skipline, sizeof(skipline)-1);
}

int main() {
    signal(SIGINT, handle_sigint);
    
    while (1) {
        // Display the prompt
        print_clp();

        // Read standard input and execute programs
        int exec = interact();

        if (sigint_received == 1) {
            sigint_received = 0;
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
