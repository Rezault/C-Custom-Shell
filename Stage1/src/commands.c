//Name: Razvan Gabriel Dobasu
//Student Number: 23363473
//I acknowledge the DCU academic integrity policy.

#include "commands.h"
#include "customshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>

// Global vars
char curr_directory[buff_size]; // Storing the current directory
FILE *fptr; // File pointer

// cd
void cmd_cd(char **args) {
	if (!args[1]) {
		printf("%s\n", curr_directory);	// No arguments, just print current directory
	} else {
		char *path = args[1];
		if (!strcmp(path, "~")) {
			path = getenv("HOME");
		}

		if (chdir(path) != 0) {
			perror("cd"); // cd failed, just show an error message
		} else {
			if (getcwd(curr_directory, sizeof(curr_directory)) != NULL) {
				setenv("PWD", curr_directory, 1); // Change PWD
			} else {
				perror("getcwd");
			}
		}
	}
}

// Clear
void cmd_clr(char **args) {
	system("clear");	
}

// Dir
void cmd_dir(char **args) {
	if (!args[1]) {
		system("ls -al"); // Just list the current directory
	} else {
		char cmd[buff_size]; // Buffer for the command

		// Using snprintf, I can print a string and redirect the output to a buffer
		// In this case, redirect to cmd
		// This way I am adding args[1] to the end of the ls statement
		snprintf(cmd, sizeof(cmd), "ls -al %s", args[1]);
		system(cmd);
	}
}

// Environ
void cmd_environ(char **args) {
	char **s = environ;
	for (; *s; s++) {
		printf("%s\n", *s);
	}
}

// Echo
void cmd_echo(char **args) {
	// Loop from i = 1 to ignore first argument which is the command name
	for (int i = 1; args[i] != NULL; i++) {
		printf("%s ", args[i]);
	}
	printf("\n");
}

// Help
void cmd_help(char **args) {
    char exe_path[buff_size], manual_path[buff_size];

    // Get the full path of the running shell executable
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("Error getting shell path");
        return;
    }
    exe_path[len] = '\0';

    // Get the directory where the executable is located
    char *shell_dir = dirname(exe_path);

    // Construct the manual path safely
    if (snprintf(manual_path, sizeof(manual_path), "%s/../manual/manual.txt", shell_dir) >= (int)sizeof(manual_path)) {
        fprintf(stderr, "Error: manual path too long\n");
        return;
    }

    // Construct command safely
    char command[buff_size];
    if (snprintf(command, sizeof(command), "more %s", manual_path) >= (int)sizeof(command)) {
        fprintf(stderr, "Error: command string too long\n");
        return;
    }

    // Execute the command
    system(command);
}



// Pause
void cmd_pause(char **args) {
	printf("Terminal paused. Press 'Enter' to continue operation.\n");
	while (getchar() != '\n');
}

// Quit
void cmd_quit(char **args) {
	if (fptr != NULL) {
		fclose(fptr);
	}
	exit(0);
}

// Unknown command
void cmd_unknown(char **args) {
	printf("Unknown command '%s'\n", args[0]);
}

// Define command table
Command commands[] = {
    {"cd", cmd_cd},
    {"clr", cmd_clr},
    {"dir", cmd_dir},
    {"environ", cmd_environ},
    {"echo", cmd_echo},
    {"help", cmd_help},
    {"pause", cmd_pause},
    {"quit", cmd_quit},
    {NULL, cmd_unknown}
};