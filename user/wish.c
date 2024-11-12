// Shell.

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAX_ARGC 10
#define MAX_LINE_LENGTH 256
#define PROMPT_STRING "wish> "
#define PROMPT_STRING_LENGTH 6


size_t strspn(const char *str, const char *delim) {
    size_t count = 0;
    while (*str) {
        const char *d = delim;
        int found = 0;
        while (*d) {
            if (*str == *d) {
                found = 1;
                break;
            }
            d++;
        }
        if (!found) {
            break;
        }
        count++;
        str++;
    }
    return count;
}


char *strpbrk(char *str, const char *delim) {
    while (*str) {
        const char *d = delim;
        while (*d) {
            if (*str == *d) {
                return str;
            }
            d++;
        }
        str++;
    }
    return NULL;
}


char *strtok_r(char *str, const char *delim, char **saveptr) {
    if (str == NULL) {
        str = *saveptr;
    }

    str += strspn(str, delim);

    if (*str == '\0') {
        return NULL;
    }

    char *token_start = str;
    str = strpbrk(token_start, delim);

    if (str) {
        *str = '\0';
        *saveptr = str + 1;
    } else {
        *saveptr = str;
    }

    return token_start;
}


int eval(int argc, char* argv[MAX_ARGC]) {
    if (argv[0] == NULL) {
        return 0;

    } else if (strcmp(argv[0], "exit") == 0) {
        exit(0);

    } else if (strcmp(argv[0], "help") == 0) {
        printf("nope.\n");

    } else {
        int pid = fork();
        if (pid < 0) {
            fprintf(2, "fork failed\n");
            exit(2);
        } else if (pid == 0) {  // Child process
            exec(argv[0], argv);  // Execute the command
            fprintf(2, "ERROR: Command failed: `%s`\n", argv[0]);  // Should never get here if exec works
            exit(2);  // Exit child process if exec fails
        } else {  // Parent process
            int time = 5000;
            wait(&time);  // Wait for the child process to finish
        }

    }
    //TODO: printf("Command not found: `%s`\n", argv[0]);
    return 0;
}


int main(int argc, char **argv) {
    char buf[MAX_LINE_LENGTH];
    int _argc;
    char* _argv[MAX_ARGC];
    char *placeholder;
    char *token;

    while (1) {
        _argc = 0;
        memset(_argv, 0, sizeof(_argv));
        printf(PROMPT_STRING);
        gets(buf, MAX_LINE_LENGTH);

        // Trim trailing newline
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        // tokenize input
        token = strtok_r(buf, " ", &placeholder);
        while (token != NULL) {
            _argv[_argc++] = token;
            token = strtok_r(NULL, " ", &placeholder);
        }

        if (eval(_argc, _argv) != 0) { break; }
    }

    return 0;
}
