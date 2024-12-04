#include "execs.h"

int exec_func(char **args, int *background) {
    // Fork to get child for new program
    child_pid = fork();

    int fd;
    if (child_pid == 0) {
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], ">") == 0) {
                // fd = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                fd = creat(args[i+1], 0644);
                if (fd == -1) {
                    perror("open");
                    return -1;
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);

                args[i] = NULL;
                break;
            }
            else if (strcmp(args[i], "<") == 0) {
                fd = open(args[i + 1], O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    return -1;
                }

                dup2(fd, STDIN_FILENO);
                close(fd);

                args[i] = NULL;
                break;
            }
        }
    }
    
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
