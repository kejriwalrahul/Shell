CC=gcc
CFLAGS=-c
DEPS=header.h

all: shell.o redirect.o
	$(CC) -o shell shell.o redirect.o -lreadline -g

shell.o: shell.c
	$(CC) $(CFLAGS) shell.c -g

redirect.o: redirect.c $(DEPS)
	$(CC) $(CFLAGS) redirect.c -g

clean:
	rm *.o shell