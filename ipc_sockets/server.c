#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>

#define PORT "5000"
#define BACKLOG 10

// ****************************************************
// Auxiliary functions
// ****************************************************

// find the number of digits in a string
int digits_in_str(char *s)
{
    int i = 0;
    while (*s != '\0') {
    	if (isdigit(*s))
       		i++;
    s++;
    }

    return i;
}

// dead child cleanup in a sigchld handler
static void sigchld_hdl(int sig)
{
	// Wait for all dead processes to finish
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	// ****************************************************
	// Variables declaration
	// ****************************************************

	int sock_fd, new_fd, gai, buf_len, sso = 1, infinite_loop = 1;
	struct addrinfo serv_addr, *serv_info, *p;
	struct sockaddr_storage client_addr;
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN], buf[BUFSIZ];
	FILE *digits;

	// ****************************************************
	// Output file 'digits.out' created
	// ****************************************************

	digits = fopen("digits.out","w");
	fclose(digits);

	// ****************************************************
	// Setup of sockets communication
	// ****************************************************

	// set server information
	memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.ai_family = AF_UNSPEC;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_flags = AI_PASSIVE; // use my IP

	// get address info and raise error if it fails
	if ((gai = getaddrinfo(NULL, PORT, &serv_addr, &serv_info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai));
		return 1;
	}

	// loop through serv_info and bind it to first option available
	for (p = serv_info; p != NULL; p = p->ai_next) {
		// raise error if socker isn't created
		if ((sock_fd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) < 0) {
			perror("server: socket wasn't created");
			continue;
		}

		// raise error if setsockopt function fails
		if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &sso,
				sizeof(int)) < 0) {
			perror("setsockopt: failed");
			exit(1);
		}

		// raise error if bind function fails
		if (bind(sock_fd, p->ai_addr, p->ai_addrlen) < 0) {
			close(sock_fd);
			perror("server: bind didn't work");
			continue;
		}

		break;
	}

	// raise error if server failed to bind
	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	// serv_info structure won't be used anymore
	freeaddrinfo(serv_info);

	// listen to incoming sockets
	if (listen(sock_fd, BACKLOG) < 0) {
		perror("listen: failed");
		exit(1);
	}

	// dead processes should be reaped
	sa.sa_handler = sigchld_hdl;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	// raise error if sigaction fails
	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		perror("server: sigaction failed");
		exit(1);
	}

	// signalize that server is waiting for connections
	printf("server: is waiting for connections\n");

	// ****************************************************
	// User enters input in an infinite loop
	// ****************************************************

	while(infinite_loop) {

		// accept incoming socket
		sin_size = sizeof client_addr;
		new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
		if (new_fd < 0) {
			perror("accept");
			continue;
		}

		// convert IPv4 and IPv6 addresses from binary to text form
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof s);

		// signalize that server got connection
		printf("server: got connection from %s\n", s);

		// ****************************************************
		// Get input string from client, count number of digits
		// and output them in the file 'digits.out'
		// ****************************************************

		// receive buffer, raise error if fail
		if ((buf_len = recv(new_fd, buf, BUFSIZ-1, 0)) < 0) {
			perror("recv: couldn't receive from client");
			exit(1);
		}

		// add '\0' symbol to char array in order to signalize end of string
		buf[buf_len] = '\0';

		// display string received in stdout
		printf("server: string received '%s'\n",buf);

		// if the input string is "quit" the infinite loop is interrupted
		if (strncmp(buf, "quit", 4) == 0) {
			infinite_loop = 0;
		}

		// the 'digits.out' file is opened so that the buffer can be appended into it
		digits = fopen("digits.out","a");

		// the number of digits in the input string is written in the output 'digits.out' file
		fprintf(digits, "%d: ", digits_in_str(buf));
		// the input string is written in the output 'digits.out' file
		fputs(buf, digits);
		// close file
		fclose(digits);
		// close socket that was accepted, so that it can accept the next one
		close(new_fd);
	}

	return 0;
}
