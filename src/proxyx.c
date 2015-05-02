#include "socket.h"
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define ANSWER "HTTP/1.1 200 OK\r\n" \
	"Connection: close\r\n" \
	"Cache-Control: no-cache, no-store\r\n\r\n" \
	"Hello World!"

void handle_client(int sock, struct sockaddr_in *sa, socklen_t sal) {
	char addr[INET6_ADDRSTRLEN+6] = {0};
	//generate a client identifier
	if (sa) {
		inet_ntop(sa->sin_family, &sa->sin_addr, addr, INET6_ADDRSTRLEN);
		sprintf(addr+strlen(addr), ":%d", ntohs(sa->sin_port));
	}
	else
		strcpy(addr, "DEBUG");
	fprintf(stderr, "%s: accepted.\n", addr);
	//real work
	write(sock, ANSWER, strlen(ANSWER));
	fprintf(stderr, "%s: sent an answer.\n", addr);
	sleep(1);
	close(sock);
	fprintf(stderr, "%s: closed.\n", addr);
}

int main(int argc, char *argv[]) {
	char *prot="http-alt";
	int o, verbose=0;
	int sock;
	int ret;
	socklen_t sal;
	while ((o=getopt(argc, argv, "p:vd")) != -1) {
		switch (o) {
			case 'p':
				prot = optarg;
				break;
			case 'd':
				handle_client(0, NULL, 0);
				return 0;
			case 'v':
				verbose++;
				//break;
		}
	}
	if ((sock=get_tcp_socket(NULL, prot, 1, &sal)) == -1)
		return 1;
	else {
		int cli;
		struct sockaddr_in *sa = malloc(sal);
		if (sa) {
			printf("Listening...\n");
			signal(SIGCHLD, SIG_IGN);
			while ((cli=accept(sock, (struct sockaddr*)sa, &sal)) != -1) {
				pid_t pid = fork();
				if (pid > 0)
					//parent
					close(cli);
				else if (!pid) {
					//child
					close(sock);
					handle_client(cli, sa, sal);
					return 0;
				}
				else {
					perror("fork");
					close(cli);
					close(sock);
					return 1;
				}
			}
			close(sock);
			return 0;
		}
		else {
			perror("malloc");
			return 1;
		}
	}
}
