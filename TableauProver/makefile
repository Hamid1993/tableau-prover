program: main.o parser.o completeTableau.o createSolutionFromTableau.o helperMethods.o
			gcc -o program main.o parser.o completeTableau.o createSolutionFromTableau.o helperMethods.o

main.o: main.c
			gcc -c main.c -o main.o

parser.o: parser.c
			gcc -c parser.c -o parser.o

completeTableau.o: completeTableau.c
			gcc -c completeTableau.c -o completeTableau.o

createSolutionFromTableu.o: createSolutionFromTableu.c
			gcc -c createSolutionFromTableau.c -o createSolutionFromTableau.o

helperMethods.o: helperMethods.c
			gcc -c helperMethods.c -o helperMethods.o

# make those two targets phony in case there exist some files with the name of clean or run
.PHONY: clean
.PHONY: run

# gets invoked from run also, no need to use this explicitly
clean: 
			rm -f *.o program
			
# rename input.txt to the name of your inputfile 			
run:  program 
			./program input.txt
			rm -f *.o program