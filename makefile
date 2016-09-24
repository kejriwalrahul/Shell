CC=gcc
CFLAGS=-c
DEPS=header.h

all: shell.o redirect.o
	$(CC) -o shell shell.o redirect.o -lreadline -g

shell.o: shell.c
	$(CC) $(CFLAGS) shell.c 

redirect.o: redirect.c $(DEPS)
	$(CC) $(CFLAGS) redirect.c

clean:
	rm *.o shell