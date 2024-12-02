#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "interact.h"

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
    // Get the username
    char *username = getenv("USER");
    if (username == NULL) {
        username = "unknown";
    }

    // Get the hostname
    char hostname[64];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        strcpy(hostname, "unknown");
    }

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    
    while (1) {
        // Get the current working directory
        char *pwd = getcwd(NULL, 0);
        if (pwd == NULL) {
            perror("error: getcwd failed");
            continue;
        }

        // Display the prompt with colors
        printf("\033[33m%s@\033[31m%s\033[0m:\033[32m%s\033[0m$ ", username, hostname, pwd);
        fflush(stdout);

        free(pwd);

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
