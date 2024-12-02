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

void handle_signal(int dummy) {
    if (child_pid > 0) {
        kill(child_pid, SIGKILL);
        waitpid(child_pid, NULL, 0);
    }
}

int main() {


    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    
    while (1) {
        // Display the prompt with colors
        print_clp();

        int exec = interact();
        // Execute the entered command
        if (exec == -1) {
            // Handle execute error if needed
            continue;
        }
        else if (exec == -100) break;
    }
    return 0;
}
