#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <unistd.h>
#define DELIMITERS " \t\n"
#define buff_size 1024
#define max_args 64

// Global vars
extern char **environ; // Environment variables
char curr_directory[buff_size]; // Storing the current directory
FILE *fptr; // File pointer

// Funcs
char *getcwd(char *buf, size_t size);
int chdir(const char *pathname);

// Initial function declarations
void cmd_cd(char **args);
void cmd_clr(char **args);
void cmd_dir(char **args);
void cmd_environ(char **args);
void cmd_echo(char **args);
void cmd_help(char **args);
void cmd_pause(char **args);
void cmd_quit(char **args);
void cmd_unknown(char **args); // Handle unknown commands

// Main command struct, this is the layout all commands follow
typedef struct {
	char *name; // Command name
	void (*func)(char **args); // Pointer to the function
} Command;

// All commands
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
    char exe_path[buff_size];  // Store the full path of the shell binary
    char manual_path[buff_size];  // Store the manual path

    // Get the full path of the running shell executable
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("Error getting shell path");
        return;
    }
    exe_path[len] = '\0';  // Null-terminate the string

    // Get the directory where the executable is located
    char *shell_dir = dirname(exe_path);

    // Construct the full path to the manual file
    snprintf(manual_path, sizeof(manual_path), "%s/../manual/manual.txt", shell_dir);

    // Use "more" to display the manual
    char command[buff_size];
    snprintf(command, sizeof(command), "more %s", manual_path);
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

// Handles command execution
void exec_command(char **args) {
	if (!args[0]) return; // No command found, just return

	// Loop through available commands
	for (int i = 0; commands[i].name != NULL; i++) {
		if (!strcmp(args[0], commands[i].name)) { 
			// Found a command, call the function associated with it
			commands[i].func(args);
			return;
		}	
	}

	// No command found
	cmd_unknown(args);
}

// Tokenise input
void tokenise(char *buff, char **args) {	
	// Tokenise input and pass into exec_command
	char *token = strtok(buff, DELIMITERS); // Create tokens
	int arg_count = 0; // Keep track of the num of arguments
	while (token != NULL && arg_count < max_args - 1) {
		args[arg_count++] = token;
		token = strtok(NULL, DELIMITERS);
	}
	args[arg_count] = NULL; // Null-terminate the array
}

int main(int argc, char*argv[]) {
	char buff[buff_size]; // Input buffer for storing user input
	char *args[max_args]; // Array storing arguments


	if (argc == 2) {
		// We have a file, read it and run the commands
		fptr = fopen(argv[1], "r");
		
		while (fgets(buff, buff_size, fptr)) {
			tokenise(buff, args);
			exec_command(args);
		}

		fclose(fptr);
		return 0;
	}

	// No file given, go to main loop
	while (!feof(stdin)) {
		if (getcwd(curr_directory, sizeof(curr_directory)) == NULL) {
			perror("getcwd");
			return 1;
		}

		printf("(%s) > ", curr_directory);
		if (fgets(buff, buff_size, stdin)) {
			tokenise(buff, args);
			exec_command(args); // Find command and execute it
		}
	}

	return 0;
}
