#include "interact.h"

void cd_func(char *arg) {
    if (arg == NULL) {
        arg = getenv("HOME");
    }

    // Attempt to change the directory
    if (chdir(arg) != 0) {
        perror("error: chdir failed");
    }

    // Update the PWD environment variable
    char *new_pwd = getcwd(NULL, 0);
    if (new_pwd != NULL) {
        setenv("PWD", new_pwd, 1);
        free(new_pwd);
    } else {
        perror("error: getcwd failed");
    }
}

int interact() {
    char *args[MAX_ARGS];

    if (read_input(args) == -1) {
        return -1;
    }

    if (signal_received == 1) {
        return -1;
    }

    // Build in programs
    if (strcmp(args[0], "exit") == 0) {
        return -100;
    }
    if (strcmp(args[0], "cd") == 0) {
        cd_func(args[1]);
        return 0;
    }

    // Exec the command
    return exec_func(args);
}

int read_input(char **args) {
    char buf[BUF_SIZE];
    ssize_t bytes_read;

    bytes_read = read(0, buf, BUF_SIZE-1);
    if (bytes_read <= 0) {
        perror("error: read");
        return -1;
    }

    buf[bytes_read] = '\0';

    if (buf[bytes_read-1] == '\n') buf[bytes_read-1] = '\0';

    int argc = 0;
    char *token = strtok(buf, " ");
    while (token != NULL && argc < MAX_ARGS-1) { 
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    args[argc] = NULL;

    return 0;
}
        
int exec_func(char **args) {
    // Fork to get child for new program
    child_pid = fork();
    
    if (child_pid == -1) {
        perror("error: forked failed");
        return -1;
    }
    else if (child_pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("error: execvp failed");
            exit(EXIT_FAILURE);
        }
    }
    else {
        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
            if (errno != EINTR) {
                perror("error: waitpid failed");
                return -1;
            }
        }
    }
    
    return 0;
}
