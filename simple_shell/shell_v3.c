#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERSIZE 128
#define BLANK_SYMBOL ' '
#define NULL_SYMBOL '\0'
#define COLON_SYMBOL ':'
#define SLASH_SYMBOL '/'
#define BLANK_STR " "
#define NULL_STR "\0"
#define COLON_STR ":"
#define SLASH_STR "/"
#define PATH "PATH"
#define CLEAR "clear\0"
#define QUIT_CMD "quit"
#define EXIT_CMD "exit"

static char *inputs[BUFFERSIZE], *paths[BUFFERSIZE];

void cmd_exec(char *cmd, char **envp)
{
	if(fork() == 0) {
		if(execve(cmd, inputs, envp) == -1) {
			printf("-shell: %s: command not found\n", cmd);
			exit(1);
		}
	} else {
		// Parent waits until child finishes executing command
		wait(NULL);
	}
}

void get_paths(char **envp_helper) {
	int i = 0;
	char *token;
	char *path_str;

	path_str = strstr(envp_helper[i], PATH);
	while(path_str == NULL) {
		i++;
		path_str = strstr(envp_helper[i], PATH);
	}

	i = 0;
	path_str += 5;

   while ((token = strsep(&path_str, COLON_STR)) != NULL) {
		paths[i] = calloc(strlen(token) + 1, sizeof(char));
		strncat(paths[i], token, strlen(token));
		strncat(paths[i], SLASH_STR, 1);
		i++;
	}
	free(token);
}

// Find the full pathname for the file 
void get_cmd_pth(char *cmd)
{
	int i;
	char *path = calloc(BUFFERSIZE, sizeof(char));
	FILE *file;

	for(i = 0; paths[i]; i++) {
		strcpy(path, paths[i]);
		strncat(path, cmd, strlen(cmd));

		if((file = fopen(path, "r"))) {
			strncpy(cmd, path, strlen(path));
			fclose(file);
		}
	}
	free(path);
}

void free_arr(char **arr)
{
	int i;
	for(i = 0 ; arr[i]; i++) {
		memset(arr[i], 0, strlen(arr[i]));
		arr[i] = NULL;
		free(arr[i]);
	}
}

char *prepare_inputs(char *input)
{
	int i = 0;
	char *token, *cmd;

	while ((token = strsep(&input,BLANK_STR)) != NULL) {
		inputs[i] = calloc(strlen(token) + 1, sizeof(char*));
		strncat(inputs[i], token, strlen(token));
		i++;
	}

	cmd = strncat(inputs[0], "\0", 1);

	return cmd;
}

void rm_substr(char *str, const char *substr)
{
  while((str = strstr(str, substr)))
    memmove(str, strlen(substr) + str, strlen(strlen(substr) + str) + 1);
}

void print_prompt(void)
{
	char hostname[BUFFERSIZE];
	char *path = calloc(BUFSIZ, sizeof(char*));
	char *username = calloc(BUFFERSIZE, sizeof(char*));

	gethostname(hostname, sizeof(hostname));
	rm_substr(hostname, ".local");

	getcwd(path, BUFSIZ);
	path = strrchr(path, '/') + 1;

	username = getlogin();

	printf("%s: %s %s$ ", hostname, path, username);
}

void initilize(char **envp) {
	char *cmd = calloc(BUFFERSIZE, sizeof(char));
	cmd = prepare_inputs(CLEAR);
	get_cmd_pth(cmd);
	cmd_exec(cmd, envp);
	free_arr(inputs);
}

void sig_hdlr(int signo)
{
	if (signo != 0)
		printf("\n");

	print_prompt();
	fflush(stdout);
}

int main(int argc, char *argv[], char *envp[]) 
{
	int fd, i;
	char c;
	char *input_str = calloc(BUFFERSIZE, sizeof(char));
	char *cmd = calloc(BUFFERSIZE, sizeof(char));

	// use ctrl+c to interrupt whatever the shell is doing
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, sig_hdlr);

	get_paths(envp);
	initilize(envp);

	// Print prompt for the first time
	sig_hdlr(0);

	// Main loop
	while(strcmp(input_str, QUIT_CMD) != 0 && strcmp(input_str, EXIT_CMD) != 0) {
		c = getchar();
		switch(c) {
			case '\n':
				if(input_str[0] != NULL_SYMBOL) {
					memset(cmd, 0, BUFFERSIZE);
					// Parse the command line
					cmd = prepare_inputs(input_str);
					get_cmd_pth(cmd);
					cmd_exec(cmd, envp);
					free_arr(inputs);
				}
				// Print the prompt string
				print_prompt();
				memset(input_str, 0, BUFFERSIZE);
				break;
			default:
				// Read the command line
				strncat(input_str, &c, 1);
				break;
		}
	}

	free(cmd);
	free(input_str);
	free_arr(paths);

	printf("\n");
	return 0;
}



    
    // shell initialization
    
    // Main loop 

        // Print the prompt string 
        
        // Read the command line and parse it 
        
        // Find the full pathname for the file 
        
        // Create a process to execute the command 
        
        // Parent waits until child finishes executing command 

    // end while 

    // Shell termination 
    
 // end main







// void cd(char *pth) {

// 	char *last_backslash;
// 	char path[BUFFERSIZE];
// 	char *token;
// 	char cwd[BUFFERSIZE]; 

// 	strcpy(path, pth);

// 	if(pth[0] != '/')
// 	{ // true for the dir in cwd
// 		getcwd(cwd ,sizeof(cwd));
// 		strcat(cwd,"/");
// 		strcat(cwd,path);
// 		chdir(cwd);
// 	} else if (pth[0] == '..') {
// 		getcwd(cwd ,sizeof(cwd));
// 		*last_backslash = strrchr(cwd, "/");
// 		*last_backslash = '\0';
// 		chdir(cwd);
// 	}
// 	} else { //true for dir w.r.t. /
// 		chdir(pth);
// 	}
// }

