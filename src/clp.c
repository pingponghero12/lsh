#include "clp.h"

char* get_username() {
    char *username = getenv("USER");
    if (username == NULL) {
        username = "unknown";
    }

    return username;
}

char* get_hostname() {
    char hostname[64];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        strcpy(hostname, "unknown");
    }

    char *result = malloc(strlen(hostname) + 1);
    if (result) {
        strcpy(result, hostname);
    }
    
    return result;
}

char* get_pwd() {
        char *pwd = getcwd(NULL, 0);
        if (pwd == NULL) {
            pwd = "error: failed to get PWD";
        }

        return pwd;
}

// Print command-line prompt
void print_clp() {
    char *username = get_username();
    char *hostname = get_hostname();
    char *pwd = get_pwd();

    printf("\033[33m%s@\033[31m%s\033[0m:\033[32m%s\033[0m$ ", username, hostname, pwd);
    fflush(stdout);

    // username points to getenv("USER") and should not be freed
    free(hostname);
    free(pwd);
}
