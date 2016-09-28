#include "header.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Print IO redirection/Piping errors
void error(char *s){
	printf("%s %s\n", "Error:", strerror(errno));
	printf("Error: %s\n", s);
	exit(0);
}

// Redirection Support
void redirect(REDIR_TYPE type, char *name)
{	
	uint fd = -1;

	switch(type){
		case IN:  fd = open(name, O_RDONLY);
				  if (fd < 0)			 error("File Open Error");
				  if(dup2(fd,IN) < 0)    error("Unable to redirect input");
				  break;
		case OUT: fd = open(name, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
				  if (fd < 0)			 error("File Open Error");
				  if(dup2(fd,OUT) < 0)   error("Unable to redirect output");
				  break;
		default:  error("Invalid redirect call");
	}
}