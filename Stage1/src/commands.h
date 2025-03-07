#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>

#define buff_size 1024

typedef struct {
    char *name;
    void (*func)(char **args);
} Command;

// Declare global variables (defined in commands.c)
extern char **environ;
extern char curr_directory[buff_size];
extern FILE *fptr;

// Function prototypes
void cmd_cd(char **args);
void cmd_clr(char **args);
void cmd_dir(char **args);
void cmd_environ(char **args);
void cmd_echo(char **args);
void cmd_help(char **args);
void cmd_pause(char **args);
void cmd_quit(char **args);
void cmd_unknown(char **args);

extern Command commands[];

#endif
