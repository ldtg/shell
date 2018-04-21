#include "builtin.h"

// returns true if the 'exit' call
// should be performed
int exit_shell(char *cmd) {

    // Your code here
    return strncmp(cmd, "exit", BUFLEN) == 0;
}

// returns true if "chdir" was performed
// this means that if 'cmd' contains:
// 	$ cd directory (change to 'directory')
// 	$ cd (change to HOME)
// it has to be executed and then return true
int cd(char *cmd) {

    // Your code here
    //TODO FIX THIS THINGGGG
    char buf[BUFLEN] = {0};
    char cmd_aux[BUFLEN] = {0};
    strncpy(cmd_aux, cmd, BUFLEN);
    if (strncmp("cd", strtok(cmd_aux, " "), 2) == 0) {
        char *aux = strtok(NULL, " ");
        if (aux == NULL) {
            char *home = getenv("HOME");
            snprintf(buf, sizeof buf, "cannot cd to %s ", home);
            if (chdir(home) < 0) {
                perror(buf);
            }
        } else {

            if (chdir(aux) < 0) {
                snprintf(buf, sizeof buf, "cannot cd to %s ", aux);
                perror(buf);
            }
        }
        return true;
    }
    return 0;
}

// returns true if 'pwd' was invoked
// in the command line
int pwd(char *cmd) {

    if (strncmp(cmd, "pwd", BUFLEN) == 0) {
        printf("%s\n", getcwd(NULL, 0));
        return true;
    }

    return 0;
}

