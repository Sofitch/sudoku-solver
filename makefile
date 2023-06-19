CC=g++
CFLAGS=-Wall -g -O

sudoku: main.o sudoku.o coords.o
	$(CC) $(CFLAGS) -o sudoku main.o sudoku.o coords.o
main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp
sudoku.o: sudoku.cpp
	$(CC) $(CFLAGS) -c sudoku.cpp
coords.o: coords.cpp
	$(CC) $(CFLAGS) -c coords.cpp

clean:
	del -f *.o sudoku.exe
