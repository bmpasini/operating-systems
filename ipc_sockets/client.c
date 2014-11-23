#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define _BSD_SOURCE
#define PORT "5000"

// ****************************************************
// Auxiliary functions
// ****************************************************

// get local IP address
void get_ip_addr (char *ip_addr)
{
	char buffer[200];
	struct hostent* host;

	gethostname(buffer, 200);
	host = (struct hostent *) gethostbyname(buffer);

	strcpy(ip_addr,inet_ntoa(*((struct in_addr *)host->h_addr)));
}

// get socket address, either IPv6 or IPv4
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

	int sockfd, gai, infinite_loop = 1;
	struct addrinfo serv_addr, *serv_info, *p;
	char ip_addr[200], s[INET6_ADDRSTRLEN];
	char *user_input = malloc(sizeof(char)*BUFSIZ);

	// ****************************************************
	// Setup of sockets communication
	// ****************************************************

	while (infinite_loop) {

		// configure server address
		memset(&serv_addr, 0, sizeof serv_addr);
		serv_addr.ai_family = AF_UNSPEC;
		serv_addr.ai_socktype = SOCK_STREAM;

		// get local IP address
		get_ip_addr(ip_addr);

		// setup address info
		if ((gai = getaddrinfo(ip_addr, PORT, &serv_addr, &serv_info)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai));
			return 1;
		}

		// loop through serv_info and bind it to first option available
		for(p = serv_info; p != NULL; p = p->ai_next) {
			// raise error if socker isn't created
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) < 0) {
				perror("client: socket wasn't created");
				continue;
			}

			// raise error if connection function fails
			if (connect(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
				close(sockfd);
				perror("client: connect didn't work");
				continue;
			}

			break;
		}

		// raise error if client failed to connect
		if (p == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			return 2;
		}

		// convert IPv4 and IPv6 addresses from binary to text form
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);

		// signalize who the client is connecting to
		printf("client: connecting to %s\n", s);

		// serv_info structure won't be used anymore
		freeaddrinfo(serv_info);

		// ****************************************************
		// Get input and send it to server
		// ****************************************************

		// user is asked for an alpha numeric string input
		printf("Enter an alpha numeric string: ");
		fgets(user_input, BUFSIZ, stdin);

		// if the input string is "quit" the infinite loop is interrupted
		if (strncmp(user_input, "quit", 4) == 0) {
			infinite_loop = 0;
		}
		
		// send input to server
		if (send(sockfd, user_input, strlen(user_input), 0) < 0)
			perror("send");

		// close socket
		close(sockfd);
	}

	return 0;
}
