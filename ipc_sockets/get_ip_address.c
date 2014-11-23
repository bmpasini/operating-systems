#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _BSD_SOURCE

// get local IP address
void get_ip_addr (char *ip_addr)
{
	char buffer[200];
	struct hostent* host;

	gethostname(buffer, 200);
	host = (struct hostent *) gethostbyname(buffer);

	strcpy(ip_addr,inet_ntoa(*((struct in_addr *)host->h_addr)));
}

int main (void)
{
	char ip_addr[200];

	get_ip_addr(ip_addr);

	printf("%s\n", ip_addr);

	return 0;
}