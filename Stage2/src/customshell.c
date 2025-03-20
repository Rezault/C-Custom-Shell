//Name: Razvan Gabriel Dobasu
//Student Number: 23363474
//I acknowledge the DCU academic integrity policy.


#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define buff_size 1024
#define max_args 64
#define DELIMITERS " \t\n"

// Handles command execution
void exec_command(char **args, char *input_file, char *output_file, int append_mode, int background) {
	if (!args[0]) return; // No command found, just return

	// Loop through available commands
	for (int i = 0; commands[i].name != NULL; i++) {
		if (!strcmp(args[0], commands[i].name)) { 
			// Found a command, call the function associated with it
			// This is a built-in command, so handle redirection
			int saved_stdout = dup(1);
			int saved_stdin = dup(0);

			if (output_file) {
				// Open the output file in write mode
				// If the file doesn't exist, we want to create it
				// Check if we need to append instead of write
				int fd = open(output_file, O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC), 0644);
				if (fd == -1) {
					perror("Error opening output file");

				 	// Restore original STDOUT and STDIN before returning
					dup2(saved_stdout, STDOUT_FILENO);
					dup2(saved_stdin, STDIN_FILENO);
					close(saved_stdout);
					close(saved_stdin);

					return;
				}
				dup2(fd, STDOUT_FILENO); // Redirect to STDOUT
				close(fd);
			}

			if (input_file) {
				int fd = open(input_file, O_RDONLY);
				if (fd == -1) {
					perror("Error opening input file");
					
				 	// Restore original STDOUT and STDIN before returning
					dup2(saved_stdout, STDOUT_FILENO);
					dup2(saved_stdin, STDIN_FILENO);
					close(saved_stdout);
					close(saved_stdin);

					return;
				}

				dup2(fd, STDIN_FILENO); // Redirect to STDIN
				close(fd);
			}
			
			// Execute built-in
			commands[i].func(args);
			
			// Restore original STDOUT and STDIN before returning
			dup2(saved_stdout, STDOUT_FILENO);
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdout);
			close(saved_stdin);

			return;
		}
	}

	// If not a built-in command, fork a child process
	pid_t pid = fork();

	if (pid < 0) {
		perror("Fork failed");
		return;
	} else if (pid == 0) {
		// Child process
		setenv("parent", getenv("shell"), 1);
		
		// Handle input redirection
		if (input_file) {
			// Open the input file as readonly
			int fd = open(input_file, O_RDONLY);
			if (fd == -1) {
				perror("Error opening input file");
				exit(1);
			}
			dup2(fd, STDIN_FILENO); // Redirect to stdin
			close(fd);
		}

		// Output redirection
		if (output_file) {
			// Create a file for output if doesn't exist, otherwise just open it
			// Check if we need to just write or append
			int fd = open(output_file, O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC), 0644);
			if (fd == -1) {
				perror("Error opening output file");
				exit(1);
			}
			dup2(fd, STDOUT_FILENO); // Redirect to the output file
			close(fd);
		}

		if (execvp(args[0], args) == -1) {
			perror("Command execution failed");
			exit(1); // Exit child process if exec fails
		}

	} else {
		// Parent process
		if (!background) {
			waitpid(pid, NULL, 0);
		} else {
			// For background processes, print the PID and immediately return to prompt
			printf("Process %d running in background\n", pid);
		}
	}

	// No command found
	//cmd_unknown(args);
}

// Tokenise input
void tokenise(char *buff, char **args, char **input_file, char **output_file, int *append_mode, int *background) {
	// Tokenise input and pass into exec_command
	char *token = strtok(buff, DELIMITERS); // Create tokens
	int arg_count = 0; // Keep track of the num of arguments

	*input_file = NULL;
	*output_file = NULL;
	*append_mode = 0; // 0 = overwrite, 1 = append
	*background = 0; // 0 = not background, 1 = background

	while (token != NULL && arg_count < max_args - 1) {
		if (strcmp(token, "<") == 0) {
			// < means we're taking in input, so set input_file
			token = strtok(NULL, DELIMITERS);
			if (token) {
				*input_file = token;
			}
		} else if (strcmp(token, ">") == 0) {
			// Write to a file, so change output_file
			token = strtok(NULL, DELIMITERS);
			if (token) {
				*output_file = token;
				*append_mode = 0;
			}
		} else if (strcmp(token, ">>") == 0) {
			// Append to a file, so change output_file and append_mode
			token = strtok(NULL, DELIMITERS);
			if (token) {
				*output_file = token;
				*append_mode = 1;
			}
		} else {
			args[arg_count++] = token; // Normal argument
		}

		token = strtok(NULL, DELIMITERS);
	}
	args[arg_count] = NULL; // Null-terminate the array

	// Check if the last token is "&" indicating background execution
	if (arg_count > 0 && strcmp(args[arg_count - 1], "&") == 0) {
		*background = 1;
		args[arg_count - 1] = NULL; // Remove the "&" token
	} else {
		*background = 0;
	}
}

int main(int argc, char*argv[]) {	
	char shell_path[buff_size];

	// Get the full path of the running shell executable
	ssize_t len = readlink("/proc/self/exe", shell_path, sizeof(shell_path) - 1);
	if (len == -1) {
		perror("Error getting shell path");
		return 1;
	}
	shell_path[len] = '\0'; // Null-terminate the string

	// Set shell environment variable
	setenv("shell", shell_path, 1);


	char buff[buff_size]; // Input buffer for storing user input
	char *args[max_args]; // Array storing arguments
	char *input_file, *output_file; // Keep track of any input/output files
	int append_mode; // Keep track if we should append or write
	int background; // Keep track of whether or not we want to run the command in the background

	// Batch mode
	if (argc == 2) {
		// We have a file, read it and run the commands
		fptr = fopen(argv[1], "r");
		if (!fptr) {
			perror("Error opening batch file");
			return 1;
		}
		
		// Read every line in the file, tokenise the command and execute it
		while (fgets(buff, buff_size, fptr)) {
			tokenise(buff, args, &input_file, &output_file, &append_mode, &background);
			exec_command(args, input_file, output_file, append_mode, background);
		}

		fclose(fptr);
		return 0;
	}

	// No file given, go to main loop
	while (!feof(stdin)) {
		if (getcwd(curr_directory, sizeof(curr_directory)) == NULL) {
			// Can't find the current directory
			perror("getcwd");
			return 1;
		}

		printf("(%s) > ", curr_directory);
		// Tokenise input and execute
		if (fgets(buff, buff_size, stdin)) {
			tokenise(buff, args, &input_file, &output_file, &append_mode, &background);
			exec_command(args, input_file, output_file, append_mode, background); // Find command and execute it
		}
	}

	return 0;
}

