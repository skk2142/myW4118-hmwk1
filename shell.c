#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
// int getline(char **, size_t *, FILE *);
// typedef struct command;

#define MAXARGS 20
#define MAXHIST 100
struct command{
	char *script_name;
	char *arguments[MAXHIST];
};

struct command *parse_command(char *);
void run_command(struct command *, char **, char *);

// extern int errno;

int main()
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	struct command *cmd;
	char *curr_dir = (char *)malloc(500);
	
	if (curr_dir==NULL)
	{
		perror("Error: memory not allocated.");
		exit(0);
	}
	strcpy(curr_dir, "");
	// char *history_array[MAXHIST] = {NULL};
	// int hist_count = 0;
	// int head = 0;
	while (1) {
		printf("$");
		read = getline(&line, &len, stdin);
		// if (read>0){
		// 	hist_count++;
		// 	history_array[array_index(head++)] = deep_copy(line);
		// }
		cmd = parse_command(line);
		if (((strcmp(cmd->script_name,"/bin/ls")==0) && ((cmd->arguments[1]==(char *)NULL)) && !(strcmp(curr_dir, "")==0)))
		{
			cmd->arguments[1] = curr_dir;
			cmd->arguments[2] = (char *)NULL;
		}
		if (((strcmp(cmd->script_name,"/bin/pwd")==0) && ((cmd->arguments[1]==(char *)NULL)) && !(strcmp(curr_dir, "")==0)))
		{
			cmd->arguments[1] = curr_dir;
			cmd->arguments[2] = (char *)NULL;
		}
		run_command(cmd, &curr_dir, line);
		free(cmd);
	};
	// free(line);
	return 0;
}



struct command *parse_command(char *line)
{
	struct command *cmd = (struct command *) malloc(sizeof(struct command));
	char *token;
	line = strtok(line, "\n");
	token = strtok(line, " ");
	int count = 0;
	if (token)
	{
		cmd->script_name = token;
		cmd->arguments[count++] = token;
	}
	while (!((token = strtok(NULL, " "))==NULL))
	{
		cmd->arguments[count++] = token;
	}
	cmd->arguments[count++] = (char *)NULL;
	return(cmd);
}

void run_command(struct command *cmd, char **curr_dir, char *line)
{
	if (strcmp(cmd->script_name,"exit")==0)
	{
		free(cmd);
		free((char*)line);
		free(*curr_dir);
		exit(0);
	}
	else if (strcmp(cmd->script_name,"cd")==0)
	{
		if ((cmd->arguments[1]))
			strcpy(*curr_dir, cmd->arguments[1]);
		else
			printf("%s\n","Error: Not a valid command");
	}
	else
	{
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			perror("Error: Fork Failed");
		}
		else if (pid == 0){
			execv(cmd->script_name, cmd->arguments);
			perror("Error: couldn't run the executable");
			exit(1);
		}
		else {
			while(pid != wait(0));	
		}

	}
}

int array_index(int i){
	if (i<0)
		return(MAXHIST+i);
	else if (i>MAXHIST-1)
		return(i-MAXHIST);
	else
		return(i);
}
