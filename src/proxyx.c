#include "socket.h"
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#define ANSWER "HTTP/1.1 200 OK\r\n" \
	"Connection: close\r\n" \
	"Cache-Control: no-cache, no-store\r\n\r\n" \
	"Hello World!"

int main(int argc, char *argv[]) {
	char *prot="http-alt";
	int o, verbose=0;
	int sock;
	int ret;
	while ((o=getopt(argc, argv, "p:v")) != -1) {
		switch (o) {
			case 'p':
				prot = optarg;
				break;
			case 'v':
				verbose++;
				//break;
		}
	}
	if ((sock=get_tcp_socket(NULL, prot, 1)) == -1)
		return 1;
	else {
		int cli;
		printf("Waiting...\n");
		while ((cli=accept(sock, NULL, NULL)) != -1) {
			printf("Accepted a connection.\n");
			write(cli, ANSWER, strlen(ANSWER));
			printf("Sent an answer.\n");
			close(cli);
			printf("Closed the connection.\n");
			printf("Waiting...\n");
		}
		close(sock);
		return 0;
	}
}
