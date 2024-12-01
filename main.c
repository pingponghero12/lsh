#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 1024
#define MAX_ARGS 64

void change_dir(char *arg) {
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

int execute() {
    char buf[BUF_SIZE];
    char *args[MAX_ARGS];
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
        
    // Build in programs
    if (strcmp(args[0], "exit") == 0) return -100;
    if (strcmp(args[0], "cd") == 0) {
        change_dir(args[1]);
        return 0;
    }

    // Fork to get child for new program
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("error: forked failed");
        return -1;
    }
    else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("error: execvp failed");
            exit(EXIT_FAILURE);
        }
    }
    else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("error: waitpid failed");
            return -1;
        }
    }
    
    return 0;
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

        int exec = execute();
        // Execute the entered command
        if (exec == -1) {
            // Handle execute error if needed
            continue;
        }
        else if (exec == -100) break;
    }
    return 0;
}
