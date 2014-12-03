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
#define NEWLINE_SYMBOL '\n'
#define COLON_SYMBOL ':'
#define SLASH_SYMBOL '/'
#define BLANK_STR " "
#define NULL_STR "\0"
#define NEWLINE_STR "\n"
#define COLON_STR ":"
#define SLASH_STR "/"
#define PARENT_DIR_STR ".."
#define PATH "PATH"
#define CD "cd"
#define CLEAR "clear\0"
#define LOCAL ".local"
#define QUIT_CMD "quit"
#define EXIT_CMD "exit"

static char *inputs[BUFFERSIZE], *paths[BUFFERSIZE];

// ****************************************************************
// * This function implements the change directory (cd) command.  *
// ****************************************************************
void cd(char *path)
{
	char last_backslash[BUFSIZ];
	char path_fix[BUFSIZ];
	char cwd[BUFSIZ];
	char *cwdptr = calloc(BUFSIZ, sizeof(char*));

	// condition for when there is no slash in the argument
	if(strncmp(path, SLASH_STR, 1) != 0 && strncmp(path, PARENT_DIR_STR, 2) != 0) {
		getcwd(cwdptr, BUFSIZ);
		strncat(cwdptr, SLASH_STR, 1);
		strncat(cwdptr, path, strlen(path));
		strncat(cwdptr, NULL_STR, strlen(path));
		strncpy(cwd,cwdptr, BUFSIZ);
		if(chdir(cwd) == -1)
			perror("chdir");
	// condition for when the argument is '..'
	} else if (strncmp(path, PARENT_DIR_STR, 2) == 0) {
		getcwd(cwd, sizeof(cwd));
		strncpy(last_backslash, strrchr(cwd, SLASH_SYMBOL), BUFFERSIZE);
		cwd[strlen(cwd)-strlen(last_backslash)] = NULL_SYMBOL;
		if(chdir(cwd) == -1)
			perror("chdir");
	} else {
		// condition for when the argument is either a full path or there
		// is no need to append a slash in the argument
		if(chdir(path) == -1) {
			getcwd(cwdptr, BUFSIZ);
			strncat(cwdptr, path, strlen(path));
			strncat(cwdptr, NULL_STR, strlen(path));
			strncpy(cwd,cwdptr, BUFSIZ);
			if(chdir(cwd) == -1)
				perror("chdir");
		}
	}
}

// ****************************************************************
// * This function was created to call the 'cd' command inside of *
// * a child process. However, the directory gets changed inside  *
// * of the child and when it gets back to the parent, the        *
// * directory change gets lost. Therefore, I didn't call this    *
// * function in the main(), I called cd() directly, so that the  *
// * 'cd' command can be used in the shell. I left the function   *
// * here just as reference.									  *
// ****************************************************************
void cd_exec(char *path) {
	// create child process
	if(!fork()) {
		cd(path);
		exit(0);
	} else {
		// parent process waits for child process termination
		wait(NULL);
	}
}

// ****************************************************************
// * This function creates a child process and executes most of   *
// * the basic commands that comes with the Operating System.     *
// ****************************************************************
void cmd_exec(char *cmd, char **envp)
{
	// create child process
	if(!fork()) {
		// execute command
		if(execve(cmd, inputs, envp) == -1) {
			printf("-shell: %s: command not found\n", cmd);
			exit(0);
		}
	} else {
		// Parent waits until child finishes executing command
		wait(NULL);
	}
}

// ****************************************************************
// * This function finds all paths in which shell commands might  *
// * be stored in the system.									  *
// ****************************************************************
void get_paths(char **envp_helper) {
	int i = 0;
	char *token;
	char *path_str;

	// find all possible paths from environment variable
	path_str = strstr(envp_helper[i], PATH);
	while(!path_str) {
		i++;
		path_str = strstr(envp_helper[i], PATH);
	}

	i = 0;
	path_str += 5;

	// separate result found above into a static array of strings
	while ((token = strsep(&path_str, COLON_STR))) {
		paths[i] = calloc(strlen(token) + 1, sizeof(char*));
		strncat(paths[i], token, strlen(token));
		strncat(paths[i], SLASH_STR, 1);
		i++;
	}
	free(token);
}

// ****************************************************************
// * This function finds the path where the input command is 	  *
// * stored in the system.										  *
// ****************************************************************
void get_cmd_pth(char *cmd)
{
	int i;
	char *path = calloc(BUFFERSIZE, sizeof(char*));
	FILE *file;

	// iterate over all possible paths
	for(i = 0; paths[i]; i++) {
		strcpy(path, paths[i]);
		strncat(path, cmd, strlen(cmd));

		// get the right path by testing it existence
		if((file = fopen(path, "r"))) {
			strncpy(cmd, path, strlen(path));
			fclose(file);
		}
	}
	free(path);
}

// ****************************************************************
// * This function frees the memory of all elements of a char	  *
// * array.														  *
// ****************************************************************
void free_arr(char **arr)
{
	int i;

	// empty all elements of array, set it to null and then set it free
	for(i = 0 ; arr[i]; i++) {
		memset(arr[i], 0, strlen(arr[i]));
		arr[i] = NULL;
		free(arr[i]);
	}
}

// ****************************************************************
// * This function parses the input into an array of strings and  *
// * returns the input command so that it can be called	by other  *
// * functions.													  *
// ****************************************************************
char *prepare_inputs(char *input)
{
	int i = 0;
	char *token, *cmd;

	// store each word of the input string into an array of strings
	while ((token = strsep(&input, BLANK_STR))) {
		inputs[i] = calloc(strlen(token) + 1, sizeof(char*));
		strncat(inputs[i], token, strlen(token));
		i++;
	}

	// return the first word of the input string as the command to be performed
	cmd = strncat(inputs[0], NULL_STR, 1);

	return cmd;
}

// ****************************************************************
// * This function removes a substring from a string.			  *
// ****************************************************************
void rm_substr(char *str, const char *substr)
{
  while((str = strstr(str, substr)))
    memmove(str, strlen(substr) + str, strlen(strlen(substr) + str) + 1);
}

// ****************************************************************
// * This function prepares the prompt and print it.			  *
// ****************************************************************
void print_prompt(void)
{
	char hostname[BUFFERSIZE];
	char *path = calloc(BUFSIZ, sizeof(char*));
	char *username = calloc(BUFFERSIZE, sizeof(char*));

	// get name of local machine
	gethostname(hostname, sizeof(hostname));
	rm_substr(hostname, LOCAL);

	// get current working directory name
	getcwd(path, BUFSIZ);
	path = strrchr(path, SLASH_SYMBOL) + 1;

	// get username
	username = getlogin();

	// print fetched information along with the prompt
	printf("%s:%s %s$ ", hostname, path, username);
}

// ****************************************************************
// * This function clears the screen, for our new shell to open   *
// * and prints the prompt for the first time.  				  *
// * Also, it gets all the paths where the system commands may be *
// * stored, by calling get_paths().							  *
// ****************************************************************
void initilize(char **envp)
{
	char *cmd = calloc(BUFFERSIZE, sizeof(char*));

	get_paths(envp);
	cmd = prepare_inputs(CLEAR);
	get_cmd_pth(cmd);
	cmd_exec(cmd, envp);
	sig_hdlr(0);
	free_arr(inputs);
}

// ****************************************************************
// * This function enables the use of the ctrl+c command to 	  *
// * interrupt whatever the shell is doing. Also, it is used to   *
// * print the promp for the first time.						  *
// ****************************************************************
void sig_hdlr(int signo)
{
	if (signo != 0)
		printf(NEWLINE_STR);
	print_prompt();
	fflush(stdout);
}

// ****************************************************************
// * The main function puts everything together so that the shell *
// * can work.													  *
// ****************************************************************
int main(int argc, char **argv, char **envp) 
{
	int fd, i;
	char c;
	char *input_str = calloc(BUFFERSIZE, sizeof(char*));
	char *cmd = calloc(BUFFERSIZE, sizeof(char*));

	// ignore the standard interrupt signal (ctrl+c)
	signal(SIGINT, SIG_IGN);
	// assign interrupt signal (ctrl+c) to the signal handler function
	signal(SIGINT, sig_hdlr);

	// prepare screen and fetch necessary data
	initilize(envp);

	// main loop might be stopped by typing 'quit', 'exit' or by pressing 'ctrl+d'
	while(strcmp(input_str, QUIT_CMD) != 0 && strcmp(input_str, EXIT_CMD) != 0 && c != EOF) {
		// get input
		c = getchar();
		// this switch-case statement read and prepare the input string
		switch(c) {
			case NEWLINE_SYMBOL:
				// if no command is inserted, just print the prompt again
				if(input_str[0] != NULL_SYMBOL) {
					// erase cmd variable
					memset(cmd, 0, BUFFERSIZE);
					// parse the command line
					cmd = prepare_inputs(input_str);
					// special case: change directory call
					if(strncmp(cmd, CD, 2) == 0) {
						cd(inputs[1]);
					// all other command calls
					} else {
						get_cmd_pth(cmd);
						cmd_exec(cmd, envp);
					}
					free_arr(inputs);
				}
				// print the prompt again after hitting enter
				print_prompt();
				memset(input_str, 0, BUFFERSIZE);
				break;
			default:
				// prepare the input string
				strncat(input_str, &c, 1);
				break;
		}
	}

	// free allocated memories
	free(cmd);
	free(input_str);
	free_arr(paths);

	// print new line if 'ctrl+d' is pressed
	if(c == EOF) printf(NEWLINE_STR);

	// end of main
	return 0;
}

