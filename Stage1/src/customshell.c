//Name: Razvan Gabriel Dobasu
//Student Number: 23363473
//I acknowledge the DCU academic integrity policy.


#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define buff_size 1024
#define max_args 64
#define DELIMITERS " \t\n"

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

