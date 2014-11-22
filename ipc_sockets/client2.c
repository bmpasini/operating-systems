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
#include <ctype.h>

#define PORT "5000" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once 

int digits_in_str(char *s)
{
    int i = 0;
    while (*s != '\0') {
    	if (isdigit(*s)) // counting number of digits
       		i++;
    s++;
    }

    return i;
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

	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	int infinite_loop = 1;
	FILE *digits;

	// ****************************************************
	// Output file 'digits.out' created
	// ****************************************************

	digits = fopen("digits.out","w");
	fclose(digits);

	// ****************************************************
	// Setup of sockets communication
	// ****************************************************

	while (infinite_loop) {

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}

			break;
		}

		if (p == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			return 2;
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);

		freeaddrinfo(servinfo); // all done with this structure

		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		    perror("recv");
		    exit(1);
		}

		buf[numbytes] = '\0';

		printf("client: received '%s'\n",buf);





		// If the input string is "quit" the infinite loop is interrupted
		if (strncmp(buf, "quit", 4) == 0) {
			infinite_loop = 0;
		}

		// The 'digits.out' file is opened so that the buffer can be appended into it
		digits = fopen ("digits.out","a");

		// The number of digits in the input string is written in the output 'digits.out' file
		fprintf(digits, "%d: ", digits_in_str(buf));
		// Then, the input string is written in the output 'digits.out' file
		fputs(buf, digits);
		// The output file is closed
		fclose(digits);
		
		




		close(sockfd);
	}

	return 0;
}
