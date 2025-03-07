#ifndef SHELL_H
#define SHELL_H

#include "commands.h"

#define buff_size 1024
#define max_args 64
#define DELIMITERS " \t\n"

// Function prototypes from shell.c
void tokenise(char *buff, char **args);
void exec_command(char **args);

#endif
