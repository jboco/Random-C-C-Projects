CC=gcc
CFLAGS=-I
program: bmpmirror.o
	$(CC) -o program bmpmirror.o -I.
bmpmirror.o: bmpmirror.c bmpfunctions.c
	$(CC) -c bmpmirror.c bmpfunctions.c