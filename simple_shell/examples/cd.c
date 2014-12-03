#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define BUFFERSIZE 512
#define BLANK_STR " "

void rm_substr(char *str, const char *substr)
{
	while((str = strstr(str, substr)))
		memmove(str, strlen(substr) + str, strlen(strlen(substr) + str) + 1);
}


void cd(char *path)
{
	char last_backslash[BUFSIZ];
	char path_fix[BUFSIZ];
	char cwd[BUFSIZ];
	char *cwdptr = calloc(BUFSIZ, sizeof(char*));

	strncpy(path_fix, path, BUFSIZ);
	path_fix[strlen(path_fix)-1] = '\0';
	strncpy(path, path_fix, BUFSIZ);

	if(strncmp(path, "/", 1) != 0 && strncmp(path, "..", 2) != 0) { // true for the dir in cwd
		getcwd(cwdptr, BUFSIZ);
		strncat(cwdptr, "/", 1);
		strncat(cwdptr, path, strlen(path));
		strncat(cwdptr, "\0", strlen(path));
		strncpy(cwd,cwdptr, BUFSIZ);
		if(chdir(cwd) == -1)
			perror("chdir");
	} else if (strncmp(path, "..", 2) == 0) {
		getcwd(cwd, sizeof(cwd));
		strncpy(last_backslash, strrchr(cwd, '/'), BUFFERSIZE);
		cwd[strlen(cwd)-strlen(last_backslash)] = '\0';
		if(chdir(cwd) == -1)
			perror("chdir");
	} else { //true for dir w.r.t. /
		if(chdir(path) == -1) {
			getcwd(cwdptr, BUFSIZ);
			strncat(cwdptr, path, strlen(path));
			strncat(cwdptr, "\0", strlen(path));
			strncpy(cwd,cwdptr, BUFSIZ);
			if(chdir(cwd) == -1)
				perror("chdir");
		}
	}
}

int main(void) {
	char *input = calloc(BUFFERSIZE, sizeof(char*));
	char cwd[BUFFERSIZE];

	getcwd(cwd, sizeof(cwd));
	printf("%s\n", cwd);

	while(1) {
		fgets(input, BUFFERSIZE, stdin);
		cd(input);
		getcwd(cwd, sizeof(cwd));
		printf("%s\n", cwd);
	}

	return 0;
}