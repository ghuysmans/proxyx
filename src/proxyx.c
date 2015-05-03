#include "socket.h"
#include "http_headers.h"
#include "http.h"
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

#define CHUNK 1024*1024

#define BADREQ "HTTP/1.1 400 Bad Request\r\n" \
	"Connection: close\r\n\r\n" \
	"Bad Request"

#define BADGW "HTTP/1.1 502 Bad Gateway\r\n" \
	"Connection: close\r\n\r\n" \
	"Bad Gateway"

#define STUPID "\r\nConnection: close\r\n\r\n"

#define STUPID2 "\r\nHost: localhost\r\nConnection: close\r\n\r\n"

void handle_client(int sock, struct sockaddr_in *sa, socklen_t sal) {
	char addr[INET6_ADDRSTRLEN+6] = {0};
	BUFFER *b = NULL;
	char *sl;
	HTTP_HEADER *h;
	char *d;
	size_t dl, r;
	//generate a client identifier
	if (sa) {
		inet_ntop(sa->sin_family, &sa->sin_addr, addr, INET6_ADDRSTRLEN);
		sprintf(addr+strlen(addr), ":%d", ntohs(sa->sin_port));
	}
	else
		strcpy(addr, "DEBUG");
	fprintf(stderr, "%s: accepted.\n", addr);
	//real work
	if (fetch_http(sock, &b, CHUNK, &sl, &h, &d, &dl, &r, 0))
		fprintf(stderr, "%s: oops.\n", addr);
	else {
		char *host = find_http_header(h, "Host");
		fprintf(stderr, "%s > %s\n", addr, sl);
		if (host) {
			//FIXME free objects
			char *hc = strdup(host);
			char *rh, *rs;
			int rem;
			if (hc) {
				get_host_parts(hc, &rh, &rs);
				rem = get_tcp_socket(rh, rs, 0, NULL);
				if (rem == -1) {
					fprintf(stderr, "%s: out get_tcp_socket.\n", addr);
					write(sock, BADGW, strlen(BADGW));
				}
				else {
					BUFFER *b2 = NULL;
					char *sl2;
					HTTP_HEADER *h2;
					char *d2;
					size_t dl2, r2;
					write(rem, sl, strlen(sl));
					write(rem, "\r\n", 2);
					send_http_headers(rem, h);
					write(rem, "\r\n", 2);
					write(rem, d, dl);
					//FIXME remaining
					if (fetch_http(rem, &b2, CHUNK, &sl2, &h2, &d2, &dl2, &r2, 1)) {
						fprintf(stderr, "%s: out fetch_http.\n", addr);
						write(sock, BADGW, strlen(BADGW));
					}
					else {
						fprintf(stderr, "%s < %s\n", addr, sl2);
						write(sock, sl2, strlen(sl2));
						write(sock, "\r\n", 2);
						send_http_headers(sock, h2);
						write(sock, "\r\n", 2);
						write(sock, d2, dl2);
						//FIXME remaining
					}
					close(rem);
				}
				free(hc);
			}
			else {
				perror("strdup");
				write(sock, BADGW, strlen(BADGW));
			}
		}
		else {
			fprintf(stderr, "%s: in Host.\n", addr);
			write(sock, BADREQ, strlen(BADREQ));
		}
	}
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
	if ((sock=get_tcp_socket(NULL, prot, 1, &sal)) < 0) {
		if (sock == -1)
			perror("get_tcp_socket");
		return 1;
	}
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
