/* ush.h -- header file for the ultra-simple shell, ush  */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <limits.h>

#ifndef MAX_CANON
#define MAX_CANON 256
#endif 
#define TRUE 1
#define FALSE 0
#define BLANK_STRING  " "
#define PROMPT_STRING ">>"
#define QUIT_STRING "q"
#define BACK_STRING "&"
#define PIPE_STRING "|"
#define NEWLINE_STRING "\n"
#define IN_REDIRECT_SYMBOL '<'
#define OUT_REDIRECT_SYMBOL '>'
#define NULL_SYMBOL '\0'
#define PIPE_SYMBOL '|'
#define BACK_SYMBOL '&'
#define NEWLINE_SYMBOL '\n'
 

/*
 * Make argv array (*argvp) for tokens in s which are separated by
 * delimiters. Return -1 on error or the number of tokens otherwise.
 */
int makeargv(char *s, char *delimiters, char ***argvp)
{
	char *t;
	char *snew;
	int numtokens;
	int i;

	/* snew is real start of string after skipping leading delimiters */
	snew = s + strspn(s, delimiters);

	/* create space for a copy of snew in t */
	if ((t = calloc(strlen(snew) + 1, sizeof(char))) == NULL) {
		*argvp = NULL;
		numtokens = -1;
	} else {
		/* count the number of tokens in snew */
		strcpy(t, snew);
		if (strtok(t, delimiters) == NULL)
			numtokens = 0;
		else
			for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++);

		/* create an argument array to contain ptrs to tokens */
		if ((*argvp = calloc(numtokens + 1, sizeof(char *))) == NULL) {
			free(t);
			numtokens = -1;

		} else {            /* insert pointers to tokens into the array */
			if (numtokens > 0) {
				strcpy(t, snew);
				**argvp = strtok(t, delimiters);
				for (i = 1; i < numtokens + 1; i++)
					*((*argvp) + i) = strtok(NULL, delimiters);
			} else {
				**argvp = NULL;
				free(t);
			}
		}
	}
   return numtokens;
}

/* ultra-simple shell, first version */
int main (int argc, char *argv[])
{
    char inbuf[MAX_BUFFER];
    char **chargv;
    int i;

    for(;;) {
	gets(inbuf);
       if (strcmp(inbuf, EXIT_STRING) == 0)
          break;
       else {
          if (fork() == 0) {
             if (makeargv(inbuf, BLANK_STRING, &chargv) > 0)
	     { 
                execvp(chargv[0], chargv);
	     }
          }
          wait(NULL);
       } 
    }   
    return 0;
}

struct command_t { 
	char *name; 
	int argc; 
	char *argv[];
};

int main() 
{ 
    
    struct command_t *command; 
    
    // shell initialization
    
    // Main loop 
    while(true) 
    { 
        // Print the prompt string 
        
        // Read the command line and parse it 
        
        // Find the full pathname for the file 
        
        // Create a process to execute the command 
        
        // Parent waits until child finishes executing command 

    } // end while 

    // Shell termination 
    
} // end main

