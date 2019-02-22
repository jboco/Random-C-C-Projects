CC=gcc
CFLAGS=-I
program: bmpreduce.o
	$(CC) -o program bmpreduce.o -I.
bmpreduce.o: bmpreduce.c bmpfunctions.c
	$(CC) -c bmpreduce.c bmpfunctions.c
