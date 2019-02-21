CC=gcc
CFLAGS=-I
program: calc1main.o calc1funcs.o
	$(CC) -o program calc1main.o calc1funcs.o -I.
calc1main.o: calc1main.c
	$(CC) -c calc1main.c
calc1funcs.o: calc1funcs.c
	$(CC) -c calc1funcs.c
