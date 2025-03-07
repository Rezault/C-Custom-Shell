# Razvan Dobasu, 23363473 - Operating Systems Project 1
# CSC1021/customshell

bin/customshell: src/customshell.c src/commands.c
	gcc src/customshell.c src/commands.c -o bin/customshell
	@echo "Build complete, run ./bin/customshell"


clean:
	rm -f bin/customshell
	@echo "Cleaned compiled files"


run: shell
	./bin/customshell
