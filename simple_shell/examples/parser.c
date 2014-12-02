#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define BUFFERSIZE 128
#define BLANK_STR " "

static char *inputs[BUFFERSIZE];

void prepare_inputs(char *input){
	int i = 0;
	char *token;

	while ((token = strsep(&input,BLANK_STR)) != NULL) {
		if (inputs[i] == NULL)
			inputs[i] = calloc(strlen(token) + 1, sizeof(char*));
		else {
			memset(inputs[i], 0, strlen(inputs[i]));
		}
		strncat(inputs[i], token, strlen(token));
		i++;
	}
}

int main(void) {
	char *input = calloc(BUFFERSIZE, sizeof(char*));

	while(1) {
		fgets(input, BUFFERSIZE, stdin);
		prepare_inputs(input);
	}
}