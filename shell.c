#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

const int size = 512;

struct command{
	char *cmd;
	char **args;
};

char *builtInCmds[] = {"exit", "pwd"};
int builtInCmdSize = 2;

int iswspace(char c){
	switch(c){
		case ' ' :
		case '\t':
		case '\r':
		case '\n': return  1;
		case '\0': return -1;

		default: return 0;
	}
}

int isBuildCommand(struct command c){
	if(c.cmd == NULL)
		return 0;

	int i = 0;
	for(;i<builtInCmdSize;i++)
		if(!strcmp(c.cmd, builtInCmds[i]))
			return i+1;

	return 0;
}

void executeBuiltInCommand(struct command c){
	int index = isBuildCommand(c) - 1;
	char cwd[1024];

	switch(index){
		case 0: exit(0);
		case 1: printf("%s\n", getcwd(cwd, sizeof(cwd)));
				break;
	}

}

void printargs(char **s, int size){
	int i;
	for(i=0; i < size ;++i)
		printf("%s\n", s[i]);
}

struct command parseCommand(char *s){
	// Array of args
	char **args = malloc(sizeof(char*)*size);
	// Index of next free arg
	int k = 0;

	// Index of next free char in buff
	int b = 0;
	char buff[size+1];
	
	int i;
	for(i=0;s[i]!='\0';i++){
		if(iswspace(s[i])){
			continue;
		}
		else{
			buff[b++] = s[i];
			if(iswspace(s[i+1])){
				buff[b] = '\0';
				args[k] = malloc(strlen(buff));
				strcpy(args[k], buff);
				k++;
				strcpy(buff, "");
				b = 0;
			}
		}
	}
	
	struct command cmd;
	cmd.cmd = args[0];
	cmd.args = args;

	// printf("Args:\n");
	// printargs(args, k);

	return cmd;
}

void printPrompt(){
	static char hostname[HOST_NAME_MAX];
	static char username[LOGIN_NAME_MAX];
	static int run = 0;
	char cwd[1024];

	if(!run){
		gethostname(hostname, HOST_NAME_MAX);
		getlogin_r(username, LOGIN_NAME_MAX);
		run = 1;
	}
	// printf("%s@%s:~%s$ ", username, hostname, getcwd(cwd, sizeof(cwd)));
	printf("%s$ ", getcwd(cwd, sizeof(cwd)));
}

// Actually invoke the child
void executeCommand(struct command c){
	if(!strcmp(c.cmd, ""))
		exit(0);

	int err = execvp(c.cmd, c.args);
	if(err == -1){
		printf("%s %s\n", "Failed to execute:", c.cmd);
		printf("%s %s\n", "Error:", strerror(errno));
		exit(0);
	}
}

int isBackgrounfJob(struct command c){
	if(c.cmd == NULL)
		return 0;

	int i;
	for(i=0;c.args[i]!=NULL;i++);
	if(!strcmp(c.args[i-1],"&"))
		return 1;
	return 0;
}

int main(int argc, char **argv){
	while(1){
		int childPid;
		char* cmdLine;
		struct command cmd;
		int stat;

		printPrompt();

		// cmdLine = readCommandLine();
		cmdLine = readline("");
		add_history (cmdLine);

		cmd = parseCommand(cmdLine);

		if(isBuildCommand(cmd)){
			executeBuiltInCommand(cmd);
		}
		else{
			childPid = fork();
			if(childPid == 0){
				executeCommand(cmd);
			}
			else{
				if(isBackgrounfJob(cmd)){
					// Record in lsb
				}
				else{
					waitpid(childPid, &stat, 0);
				}
			}
		}
	}
}