#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFERSIZE 128
#define BLANK_STR " "

static char *inputs[BUFFERSIZE];

// Find the full pathname for the file 
int get_cmd(char *cmd)
{
	int i;
	char *path;
	FILE * file;

	for(i = 0; paths[i] != NULL; i++) {
		strcpy(path, paths[i]);
		strncat(path, cmd, strlen(cmd));

		if((file = fopen(path, "r")) > 0) {
			strncpy(cmd, path, strlen(path));
			fclose(file);
			return 0;
		}
	}
	return 0;
}