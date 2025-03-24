# Name: Razvan Gabriel Dobasu
# Student Number: 23363473
# I acknowledge the DCU academic integrity policy.

bin/customshell: src/customshell.c src/commands.c
	mkdir -p bin
	gcc src/customshell.c src/commands.c -o bin/customshell
	@echo "Build complete, run ./bin/customshell"


clean:
	rm -f bin/customshell
	@echo "Cleaned compiled files"
