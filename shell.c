#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>
#include "header.h"

// Constant defininng max length of command typed on console
const int size = 512;

// Defining list of inbuilt commands that shell executes (not via child process)
char *builtInCmds[] = {"exit", "pwd"};
int builtInCmdSize = 2;

// Define characters that delimit words in input 
int iswhspace(char c){
	switch(c){
		case ' ' :
		case '\t':
		case '\r':
		case '\n': return  1;
		case '\0': return -1;

		default: return 0;
	}
}

// Function to check if given command is inbuilt into shell or not
int isBuildCommand(struct command c){
	// If empty command, do nothing
	if(c.cmd == NULL)
		return 0;

	// Compare c's prog with inbuilt commands
	int i = 0;
	for(;i<builtInCmdSize;i++)
		if(!strcmp(c.cmd, builtInCmds[i]))
			// Return (index+1) in builtInCmds if c's prog is inbuilt command
			return i+1;

	// if not inbuilt return 0
	return 0;
}

// Function to execute inbuilt commands
void executeBuiltInCommand(struct command c){
	// Get index in builtInCmds of c's command
	int index = isBuildCommand(c) - 1;
	// Buffer to store current working directory for 'pwd'
	char cwd[1024];

	// Execute inbuilt commands
	switch(index){
		case 0: exit(0);
		case 1: printf("%s\n", getcwd(cwd, sizeof(cwd)));
				break;
	}

}

// Print tokens in command
// Function for debugging only
void printargs2(char **s){
	int i;
	for(i=0; s[i] ;++i)
		printf("%s\n", s[i]);
}

// Print tokens in command
// Function for debugging only
void printargs(char **s, int size){
	int i;
	for(i=0; i < size ;++i)
		printf("%s\n", s[i]);
}

// Command parser
struct command* parseCommand(char *s){
	// 'par'  holds head of command linked list, i.e., it contains the first command in list of commands
	struct command *par  = malloc(sizeof(struct command)); 
	// 'curr' refers to current element in linked list of commands
	struct command *curr = par;
	// Initialize next with NULL
	curr->next = NULL;

	// Array of args
	// Allocate memory for pointer to sufficient no of tokens
	curr->args = malloc(size*sizeof(char*));
	// Index of next free arg
	int k = 0;

	// Index of next free char in buff
	int b = 0;
	// buff holds current token being built
	char buff[size+1];
	
	int i;
	for(i=0;s[i]!='\0';i++){
		// ignore whitespace
		if(iswhspace(s[i]))
			continue;

		// if current command is terminated
		if(s[i] == ';'){
			// store prog name in corresponding field
			curr->cmd		= curr->args[0];
			// store separator
			curr->separator = ';';
			// init new node in linked list
			curr->next 		= malloc(sizeof(struct command));
			// move to new node
			curr 			= curr->next;
			// initialize args token list
			curr->args 		= malloc(size*sizeof(char*));
			// reset args index counter
			k = 0;
			// init next element to NUll
			curr->next 		= NULL;
			// move on to next char in input
			continue;
		}

		// append char to buffer
		buff[b++] = s[i];

		// if current token is finished, add to token list
		// and reinit buff
		if(iswhspace(s[i+1]) || s[i+1]==';'){
			// terminate current token with '\0'
			buff[b] = '\0';
			// allocate memory for string in args element
			curr->args[k] = malloc(strlen(buff));
			// copy token string
			strcpy(curr->args[k], buff);
			// increment index counter
			k++;
			// reset buffer
			strcpy(buff, "");
			b = 0;
		}
	}

	// store prog name in corresponding field
	curr->cmd = curr->args[0];
	// printf("Args:\n");
	// printargs(args, k);

	// returns first element in command linked list
	return par;
}

// Prints prompt on console
void printPrompt(){
	static char hostname[HOST_NAME_MAX];
	static char username[LOGIN_NAME_MAX];
	static int run = 0;
	char cwd[1024];

	// If function executed first time
	if(!run){
		// Gets computer name
		gethostname(hostname, HOST_NAME_MAX);
		// Gets user name
		getlogin_r(username, LOGIN_NAME_MAX);
		run = 1;
	}
	// Deploy prompt:
	// printf("%s@%s:~%s$ ", username, hostname, getcwd(cwd, sizeof(cwd)));
	
	// Test prompt: (to distinguish between real shell and our shell)
	printf("%s$ ", getcwd(cwd, sizeof(cwd)));
}

// Actually invoke the child
void executeCommand(struct command c, int *sharedPipe){
	// If empty command, terminate child
	if(!strcmp(c.cmd, ""))
		exit(0);

	// int i;
	// for(i=0;c.args[i]!=NULL;i++) {
	// 	if ()
	// }

	// Execute command on child, store return val in case of failure
	int err = execvp(c.cmd, c.args);
	// Print error msg corresponding to errno
	if(err == -1){
		printf("%s %s\n", "Failed to execute:", c.cmd);
		printf("%s %s\n", "Error:", strerror(errno));
		exit(0);
	}
}

// Check if command is to be executed on background
int isBackgrounfJob(struct command c){
	// If empty, do nothing
	if(c.cmd == NULL)
		return 0;

	// traverse to end of token list
	int i;
	for(i=0;c.args[i]!=NULL;i++);
	// If last token was '&', then must be background process
	// need to confirm this	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CHECK THIS
	if(!strcmp(c.args[i-1],"&"))
		return 1;
	
	// Else not background process
	return 0;
}

// Do infinitely
int main(int argc, char **argv){
	while(1){
		int childPid;
		char *cmdLine;
		struct command *cmd;
		int stat;

		// prints prompt
		printPrompt();

		// reads input line
		cmdLine = readline("");
		// Stores it in history
		add_history (cmdLine);

		// parses given input
		cmd = parseCommand(cmdLine);

		// while commands exists in given ip
		while(cmd){
			// If inbuilt command, let shell execute it
			if(isBuildCommand(*cmd)){
				executeBuiltInCommand(*cmd);
			}
			// If external prog
			else{
				// Fork a child
				int sharedPipe[2];

				childPid = fork();
				// In child
				if(childPid == 0){
					int i=0;
					for(i=0;cmd->args[i]!=NULL;i++) {
						if (strcmp("<",cmd->args[i]) == 0)
							redirect(IN, cmd->args[i+1]);
						else if (strcmp(">",cmd->args[i]) == 0)
							redirect(OUT, cmd->args[i+1]);
						else if (strcmp("|",cmd->args[i]) == 0);
						else if (strcmp("&",cmd->args[i]) == 0);
					}
					// Execute program
					executeCommand(*cmd, sharedPipe);
				}
				// In Parent
				else{
					// Check if background process
					if(isBackgrounfJob(*cmd)){
						// Record in lsb
						// Handle background processes here <<<<<<<<<<<<<<<<<< IMPLEMENT
					}
					// If not background process, wait for child to terminate
					else{
						waitpid(childPid, &stat, 0);
					}
				}
			}
			// Move to next command in command linked list
			cmd = cmd->next;
		}
	}
}