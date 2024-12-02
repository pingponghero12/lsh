#include "signals.h"

void handle_sigint(int dummy) {
    signal_received = 1;

    if (child_pid > 0) {
        kill(child_pid, SIGINT);
        waitpid(child_pid, NULL, WNOHANG);
    }

    const char skipline[] = "\n";
    write(STDOUT_FILENO, skipline, sizeof(skipline)-1);
}

void handle_sigterm(int dummy) {
    signal_received = 1;

    if (child_pid > 0) {
        kill(child_pid, SIGTERM);
        waitpid(child_pid, NULL, WNOHANG);
    }

    const char newline[] = "\n";
    write(STDOUT_FILENO, newline, sizeof(newline) - 1);
}

void handle_sigquit(int dummy) {
    signal_received = 1;

    if (child_pid > 0) {
        kill(child_pid, SIGQUIT);
        waitpid(child_pid, NULL, WNOHANG);
    }

    const char newline[] = "\n";
    write(STDOUT_FILENO, newline, sizeof(newline) - 1);
}

void handle_sigtstp(int dummy) {
    signal_received = 1;

    if (child_pid > 0) {
        kill(child_pid, SIGTSTP);
    }

    const char newline[] = "\n";
    write(STDOUT_FILENO, newline, sizeof(newline) - 1);
}
