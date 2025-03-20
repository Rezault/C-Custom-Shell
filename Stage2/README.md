# OVERVIEW

A simple command-line shell implemented in C for CSC1021 project.

In Stage 1, the shell supported a series of internal commands (cd, clr, dir, environ, echo, help, pause, and quit), as well as batch mode to read commands from a file.
Now, in Stage 2, functionality is extended by:


- Executing external programs (e.g., ls, gcc, python3) through fork() and execvp().
- Redirecting input and output using <, >, >>.
- Running programs in the background using the ampersand &.

# INSTRUCTIONS
- Compile the shell using:
> make

The compiled binary will appear in the **bin/** directory (**./bin/customshell**).

- Run the shell interactively:
> ./bin/customshell

or

> make run

- Run the shell in batch mode:
> ./bin/customshell file

The shell reads and executes commands inside of the file provided.

# ENVIRONMENT VARIABLES
- **shell**: Full path of the shell executable.
- **parent**: For external commands, we set **parent** to the same value as **shell**, indicating this shell is the parent process. 
- **PWD**: The current working directory. Updates everytime you run **cd**.

# I/O REDIRECTION
The shell supports redirecting input and output for both built-in and external commands.
1. **< inputfile**: Redirects standard input from inputfile.
2. **> outputfile**: Redirects standard output to outputfile.
3. **>> outputfile**: Appends standard output to outputfile instead of overwriting.

# BACKGROUND EXECUTION
The ampersand (&) at the end of a command is used to run external commands in the background.
The shell immediately returns to the prompt while the command continues running in the background.

e.g.
> python3 script.py &

Will run script.py in the background, while the user can continue using the shell.

USER MANUAL
-------------------------
cd - Changes to a specified directory.

clr - Clears the screen.

environ - Lists all environment strings.

echo - Print something onto the screen.

help - Displays a basic user manual.

pause - Pause operation of the terminal until 'Enter' is pressed.

quit - Quits the terminal.


**Enjoy using this custom shell!**