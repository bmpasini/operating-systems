
void run_cmd(char *cmd, char **envp)
{
	strncat(cmd, "\0", 1);


	if(!(strchr(cmd, '/'))) {
		// Find the full pathname for the file
		if(get_cmd(cmd))
			// Create a process to execute the command
			run_cmd(cmd, envp);
	} else {
		if((fd = open(cmd, O_RDONLY)) == -1) {
			close(fd);
			run_cmd(cmd, envp);
		}
	}

	

	cmd = get_cmd(cmd);
	run_cmd(cmd, envp);

	if(fork() == 0) {
		if(execve(cmd, inputs, envp) == -1) {
			printf("-shell: %s: command not found\n", cmd);
			exit(1);
		}
	} else {
		wait(NULL);
	}
}