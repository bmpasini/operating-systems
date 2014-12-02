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
#define NULL_STR "\0"
#define SLASH_STR "/"
#define PATH "PATH"

static char *parsed_argv[BUFFERSIZE], *parsed_envp[BUFFERSIZE], *argv_path[BUFFERSIZE];

void run_cmd(char *cmd)
{
	if(fork() == 0) {
		if(execve(cmd, parsed_argv, parsed_envp) == -1) {
			printf("%s: command not found\n", cmd);
			exit(1);
		}
	} else {
		wait(NULL);
	}
}

void find_path(char **envp_search, char *bin_path)
{
	int i = 0;
	char *tmp;

	while(1) {
		tmp = strstr(envp_search[i], PATH);
		if(tmp == NULL) {
			i++;
		} else {
			break;
		}
	}
    strncpy(bin_path, tmp, strlen(tmp));
}

// parse different paths and store them in a variable which is used to determine existence of paths
void parse_paths(char *path_str) 
{
	int i = 0;
	char str[BUFFERSIZE];
	char *path_str_ptr = path_str + 5;
	// char *path_str_ptr = path_str;

	// while(*path_str_ptr != '=')
	// 	path_str_ptr++;
	// path_str_ptr++;

	while(*path_str_ptr != NULL_SYMBOL) {
		if(*path_str_ptr == COLON_SYMBOL) {
			strncat(str, SLASH_STR, 1);
			argv_path[i] = calloc(strlen(str) + 1, sizeof(char));
			strncat(argv_path[i], str, strlen(str));
			strncat(argv_path[i], NULL_STR, 1);
			memset(str, 0, BUFFERSIZE);
			i++;
		} else {
			strncat(str, path_str_ptr, 1);
		}
		path_str_ptr++;
	}
}

// Find the full pathname for the file 
int attach_path(char *cmd)
{
	int i;
	int fd;
	char str[BUFFERSIZE];

	memset(str, 0, BUFFERSIZE);

	for(i = 0; argv_path[i] != NULL; i++) {
		strcpy(str, argv_path[i]);
		strncat(str, cmd, strlen(cmd));

		if((fd = open(str, O_RDONLY)) > 0) {
			strncpy(cmd, str, strlen(str));
			close(fd);
			return 0;
		}
	}
	return 0;
}

void alloc_envp(char **envp_str)
{
	int i;
	for(i = 0; envp_str[i] != NULL; i++) {
		parsed_envp[i] = calloc(strlen(envp_str[i]) + 1, sizeof(char));
		memcpy(parsed_envp[i], envp_str[i], strlen(envp_str[i]));
	}
}


void argv_reset(void)
{
	int i;
	// review why all this
	for(i = 0 ; parsed_argv[i] != NULL; i++) {
		memset(parsed_argv[i], 0, strlen(parsed_argv[i]) + 1);
		parsed_argv[i] = NULL;
		free(parsed_argv[i]);
	}
}

void argv_parser(char *argv_str)
{
	int i = 0;
	char str[BUFFERSIZE];
	char *argv_str_ptr = argv_str;

	memset(str, 0, BUFFERSIZE);

	while (*argv_str_ptr != NULL_SYMBOL) {
		if (*argv_str_ptr == BLANK_SYMBOL) {
			if (parsed_argv[i] == NULL)
				parsed_argv[i] = calloc(strlen(str) + 1, sizeof(char));

			else {
				memset(parsed_argv[i], 0, strlen(parsed_argv[i]));
			}
			strncpy(parsed_argv[i], str, strlen(str));
			strncat(parsed_argv[i], NULL_STR, 1);
			memset(str, 0, BUFFERSIZE);
			i++;
		} else {
			strncat(str, argv_str_ptr, 1);
		}
		argv_str_ptr++;
	}
	parsed_argv[i] = calloc(strlen(str) + 1, sizeof(char));
	strncpy(parsed_argv[i], str, strlen(str));
	strncat(parsed_argv[i], "\0", 1);
}

void sig_hdlr(int signo)
{
	printf("\n$ ");
	fflush(stdout);
}

int main(int argc, char *argv[], char *envp[]) 
{
	int fd, i;
	char c;
    char *tmp = calloc(BUFFERSIZE, sizeof(char));
    char *cmd = calloc(BUFFERSIZE, sizeof(char));
    char *path_str = calloc(BUFFERSIZE, sizeof(char));

    // use ctrl+c to interrupt whatever the shell is doing
    signal(SIGINT, SIG_IGN);
	signal(SIGINT, sig_hdlr);

	alloc_envp(envp);
	find_path(parsed_envp, path_str);
	parse_paths(path_str);

	if(fork() == 0) {
		execve("/usr/bin/clear", argv, parsed_envp);
		exit(1);
	} else {
		wait(NULL);
	}

	// Print the prompt string for the first time
    printf("$ ");
	fflush(stdout);

	// Main loop
    while(c != EOF) {
		c = getchar();
		switch(c) {
			case '\n':
				if(tmp[0] == NULL_SYMBOL) {
					printf("$ ");
				} else {
					// Parse the command line
					argv_parser(tmp);
					strncpy(cmd, parsed_argv[0], strlen(parsed_argv[0]));
					strncat(cmd, "\0", 1);
					if(strchr(cmd, '/') == NULL) {
						// Find the full pathname for the file
						if(attach_path(cmd) == 0) {
							// Create a process to execute the command
							run_cmd(cmd);
						} else {
							printf("%s: command not found\n", cmd);
						}
					} else {
						if((fd = open(cmd, O_RDONLY)) == -1) {
							close(fd);
							run_cmd(cmd);
						} else {
							printf("%s: command not found\n", cmd);
						}
					}
					// Parent waits until child finishes executing command
					// ????????????

					argv_reset();
					// Print the prompt string
					printf("$ ");
					memset(cmd, 0, BUFFERSIZE);
				}
				memset(tmp, 0, BUFFERSIZE);
				break;
			default:
				// Read the command line
				strncat(tmp, &c, 1);
				// printf("%s\n", tmp);
				break;
		}
	}
	free(tmp);
	free(path_str);

	for(i = 0; parsed_envp[i] != NULL;i++)
		free(parsed_envp[i]);

	for(i=0; i < BUFFERSIZE; i++)
		free(argv_path[i]);

	printf("\n");
	return 0;
}









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

