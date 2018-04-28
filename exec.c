#include "exec.h"


// sets the "key" argument with the key part of
// the "arg" argument and null-terminates it
static void get_environ_key(char *arg, char *key) {

    int i;
    for (i = 0; arg[i] != '='; i++)
        key[i] = arg[i];

    key[i] = END_STRING;
}

// sets the "value" argument with the value part of
// the "arg" argument and null-terminates it
static void get_environ_value(char *arg, char *value, int idx) {

    int i, j;
    for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
        value[j] = arg[i];

    value[j] = END_STRING;
}

// sets the environment variables passed
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void set_environ_vars(char **eargv, int eargc) {
    char key[ARGSIZE];
    char value[ARGSIZE];

    for (int i = 0; i < eargc; ++i) {
        get_environ_key(eargv[i], key);
        get_environ_value(eargv[i], value, block_contains(eargv[i], '='));
        if (setenv(key, value, OVERWRITE_ON_EXIST) < 0)
            perror("ERROR on setenv");
    }
}

// opens the file in which the stdin/stdout or
// stderr flow will be redirected, and returns
// the file descriptor
// 
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int open_redir_fd(char *file) {

    // Your code here
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
void exec_cmd(struct cmd *cmd) {
    switch (cmd->type) {

        case EXEC: {
            struct execcmd *execcmd = (struct execcmd *) cmd;
            set_environ_vars(execcmd->eargv, execcmd->eargc);
            execvp(execcmd->argv[0], execcmd->argv);
            //Reaches only if error happens
            perror(execcmd->scmd);
            _exit(-1);
            break;
        }

        case BACK: {
            // runs a command in background
            struct backcmd *back = (struct backcmd *) cmd;
            exec_cmd(back->c);
            //Reaches only if error happens
            perror(back->scmd);
            _exit(-1);
            break;
        }

        case REDIR: {
            // changes the input/output/stderr flow
            struct execcmd *execcmd = (struct execcmd *) cmd;

            if (strnlen(execcmd->in_file, FILENAME_MAX) > 0) {
                int in_fd = open(execcmd->in_file, O_RDONLY);
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (strnlen(execcmd->out_file, FILENAME_MAX) > 0) {
                if (execcmd->out_file[0] == '&') {
                    switch (execcmd->out_file[1]) {
                        case '2': {
                            int out_fd = STDERR_FILENO;
                            dup2(out_fd, STDOUT_FILENO);
                            break;
                        }
                        default:
                            break;
                    }
                } else {
                    int out_fd = creat(execcmd->out_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                }
            }
            if (strnlen(execcmd->err_file, FILENAME_MAX) > 0) {
                if (execcmd->err_file[0] == '&') {
                    switch (execcmd->err_file[1]) {
                        case '1': {
                            int err_fd = STDOUT_FILENO;
                            dup2(err_fd, STDERR_FILENO);
                            break;
                        }
                        default:
                            break;
                    }
                } else {
                    int err_fd = creat(execcmd->err_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                    dup2(err_fd, STDERR_FILENO);
                    close(err_fd);
                }
            }

            set_environ_vars(execcmd->eargv, execcmd->eargc);
            execvp(execcmd->argv[0], execcmd->argv);

            //Reaches only if error happens
            perror(execcmd->scmd);
            _exit(-1);
            break;
        }

        case PIPE: {
            // pipes two commands
            int filedes[2];
            pipe(filedes);

            struct pipecmd *pcmd = (struct pipecmd *) cmd;

            pid_t pid_a, pid_b;

            if (!(pid_a = fork())) {
                dup2(filedes[1], STDOUT_FILENO);
                close(filedes[0]);
                close(filedes[1]);
                exec_cmd(pcmd->leftcmd);
            }

            if (!(pid_b = fork())) {
                dup2(filedes[0], STDIN_FILENO);
                close(filedes[0]);
                close(filedes[1]);
                exec_cmd(pcmd->rightcmd);
            }
            close(filedes[0]);
            close(filedes[1]);
            waitpid(pid_a, NULL, 0);
            waitpid(pid_b, NULL, 0);

            // free the memory allocated
            // for the pipe tree structure
            free_command(parsed_pipe);

            break;
        }
    }
}

