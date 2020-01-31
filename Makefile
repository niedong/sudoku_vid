objects = main.o sudoku_main.o sudoku.o
sudoku_vid: $(objects)
	gcc -o sudoku_vid $(objects)
main.o: sudoku_main.h
	gcc -c main.c
sudoku_main.o: sudoku.h sudoku_main.h
	gcc -c sudoku_main.c
sudoku.o: sudoku.h
	gcc -c sudoku.c
clean: rm sudoku_vid $(objects)
