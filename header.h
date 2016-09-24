#ifndef HEADER_H
#define HEADER_H
typedef enum {
	IN,
	OUT
}REDIR_TYPE;


struct command{
	char *cmd;
	char **args;
	char separator;
	struct command *next;
};

void redirect(REDIR_TYPE, char *);
#endif