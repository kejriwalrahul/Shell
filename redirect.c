#include "header.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void redirect(REDIR_TYPE type, char *name)
{
	if (name == NULL) {
		puts("file does not exist");
		exit(0);
	}
	uint fd = -1;
	switch(type) {
		case IN:
			close(IN);
			fd = open(name, O_RDONLY);
			break;
		case OUT:
			close(OUT);
			fd = open(name, O_WRONLY);
			break;
	}
	if (fd == -1) {
		puts("error in opening file");
		exit(0);
	}
}

void pipline(char* fout, char* fin, int pid)
{
	int p[2];
}