#ifndef HEADER_H
#define HEADER_H

typedef enum {
	IN,
	OUT
} REDIR_TYPE;

// struct to encode each separate command
struct command{
	char *cmd;				//String defining prog name
	char **args;			//Array of strings where 0th element is prog name 
							//and subsequent elements are tokens typed after prog name including |, <> and &
	char separator; 		// contains ';' if command was terminated by ;
	struct command *next;	//Points to next command if multiple commands were typed on same line separated by ';'
};

void redirect(REDIR_TYPE, char *);
void error(char *s);

#endif