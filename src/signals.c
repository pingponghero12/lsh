#include "signals.h"

void handle_sigint(int dummy) {
    sigint_received = 1;

    if (child_pid > 0) {
        kill(child_pid, SIGINT);
        waitpid(child_pid, NULL, WNOHANG);
    }

    const char skipline[] = "\n";
    write(STDOUT_FILENO, skipline, sizeof(skipline)-1);
}

