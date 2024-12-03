#include "interact.h"

#define MAX_CMDS 10  // Maximum number of piped commands

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
    printf("number of commands: %d\n", num_cmds);

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
int exec_func(char **args, int *background) {
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
        if (*background == 1) {
            *background = 0;
            return 0;
        }
        if (waitpid(child_pid, &status, 0) == -1) {
            if (errno != EINTR) {
                perror("error: waitpid failed");
                return -1;
            }
        }
    }
    return 0;
}

int exec_pipeline(char ***cmds, int num_cmds, int background) {
    int pipefd[MAX_CMDS - 1][2];
    pid_t pids[MAX_CMDS];

    // Create the necessary pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("error: pipe");
            return -1;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("error: fork");
            return -1;
        } 
        else if (pids[i] == 0) {
            // Redirect input from previous pipe if not the first
            if (i > 0) {
                if (dup2(pipefd[i - 1][0], STDIN_FILENO) == -1) {
                    perror("error: dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Redirect output to next pipe if not the last
            if (i < num_cmds - 1) {
                if (dup2(pipefd[i][1], STDOUT_FILENO) == -1) {
                    perror("error: dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors in the child
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }

            // Exec
            if (execvp(cmds[i][0], cmds[i]) == -1) {
                perror("error: execvp");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Parent closes all pipefd
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    // Wait for child processes if not running in background
    if (!background) {
        for (int i = 0; i < num_cmds; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    // Free allocated memory for commands
    for (int i = 0; i < num_cmds; i++) {
        for (int j = 0; cmds[i][j] != NULL; j++) {
            free(cmds[i][j]);
        }
        free(cmds[i]);
    }

    return 0;
}
