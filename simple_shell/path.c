void get_paths(char **envp_helper) {
	int i = 0;
	char *tmp;
	char *token;
	char str[BUFFERSIZE];
	char *path_str = calloc(BUFFERSIZE, sizeof(char));
	char *path_str_dup;

	tmp = strstr(envp_helper[i], PATH);
	while(tmp == NULL) {
		i++;
		tmp = strstr(envp_helper[i], PATH);
	}

	strncpy(path_str, tmp, strlen(tmp));
	path_str_dup = strdup(path_str);
	path_str_dup += 5;

	printf("%s\n", path_str_dup);

   while ((token = strsep(&path_str_dup, COLON_STR)) != NULL) {
		paths[i] = calloc(strlen(token) + 1, sizeof(char));
		strncat(paths[i], token, strlen(token));
		strncat(paths[i], SLASH_STR, 1);
		printf("%s\n", paths[i]);
		i++;
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


 