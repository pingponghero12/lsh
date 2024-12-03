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
    char **cmds[MAX_CMDS];
    int num_cmds = 0;
    int background = 0;

    if (read_input(cmds, &num_cmds, &background) == -1) {
        return -1;
    }

    if (signal_received == 1) {
        return -1;
    }

    // Handle built-in commands if there's only one command and no pipeline
    if (num_cmds == 1) {
        char **args = cmds[0];
        if (args[0] == NULL) {
            // Empty command
            return 0;
        }

        if (strcmp(args[0], "exit") == 0) {
            return -100;
        }
        if (strcmp(args[0], "cd") == 0) {
            cd_func(args[1]);
            return 0;
        }
    }

    // Execute the pipeline of commands
    return exec_pipeline(cmds, num_cmds, background);
}

int read_input(char ***cmds, int *num_cmds, int *background) {
    char buf[BUF_SIZE];
    ssize_t bytes_read;

    bytes_read = read(STDIN_FILENO, buf, BUF_SIZE - 1);
    if (bytes_read <= 0) {
        perror("error: read");
        return -1;
    }

    buf[bytes_read] = '\0';

    if (buf[bytes_read - 1] == '\n') buf[bytes_read - 1] = '\0';

    // Split input into commands based on '|'
    // Using strtok_r since we want to avoid conflict of strtoks between cmd and args
    char *cmd_saveptr;
    char *cmd_str = strtok_r(buf, "|", &cmd_saveptr);
    *num_cmds = 0;

    while (cmd_str != NULL && *num_cmds < MAX_CMDS) {
        cmds[*num_cmds] = malloc(MAX_ARGS * sizeof(char *));
        int argc = 0;

        // Tokenize each command into arguments
        char *token_saveptr;
        char *token = strtok_r(cmd_str, " ", &token_saveptr);
        while (token != NULL && argc < MAX_ARGS - 1) {
            if (strcmp(token, "&") == 0) {
                *background = 1;
            } else {
                cmds[*num_cmds][argc++] = strdup(token);
            }
            token = strtok_r(NULL, " ", &token_saveptr);
        }
        cmds[*num_cmds][argc] = NULL;
        (*num_cmds)++;
        cmd_str = strtok_r(NULL, "|", &cmd_saveptr);
    }

    return 0;
}
