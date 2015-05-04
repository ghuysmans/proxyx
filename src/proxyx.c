#include "socket.h"
#include "http_headers.h"
#include "http.h"
#include "cache.h"
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define CHUNK 1024*1024
#define HTTP_OK "HTTP/1.1 2"

#define BADREQ "HTTP/1.1 400 Bad Request\r\n" \
	"Connection: close\r\n\r\n" \
	"Bad Request"

#define BADGW "HTTP/1.1 502 Bad Gateway\r\n" \
	"Connection: close\r\n\r\n" \
	"Bad Gateway"

/**
 * Transmits a response from a remote server
 */
void transmit_response(char *addr, char *sl2, int sock, HTTP_HEADER *h2, char *d2, size_t dl2) {
	fprintf(stderr, "%s < %s\n", addr, sl2);
	write(sock, sl2, strlen(sl2));
	write(sock, "\r\n", 2);
	send_http_headers(sock, h2);
	write(sock, "\r\n", 2);
	write(sock, d2, dl2);
	//FIXME remaining
}

/**
 * Makes a request to a remote server and forwards it to the client.
 * FIXME not exactly that, should be rephrased.
 */
void make_request(char *addr, int *last_socket, char **last_host, char *host, char *rh, char *rs, BUFFER **b2, int sock, char *sl, HTTP_HEADER *h, char *d, size_t dl) {
	char generic[CACHE_FILENAME_LENGTH] = {0};
	char precise[CACHE_FILENAME_LENGTH] = {0};
	char *sl2;
	HTTP_HEADER *h2;
	char *d2;
	size_t dl2, r2;
	int f;
	cache_generic(generic, sl);
	if (generic[0] && (f = open(generic, O_RDONLY))!=-1) {
		//the request is cachable and we got a generic cached response
		if (fetch_http(f, b2, CHUNK, &sl2, &h2, &d2, &dl2, &r2, 1)) {
			fprintf(stderr, "%s: cache_generic fetch_http.\n", addr);
			close(f);
		}
		else {
			int f2;
			cache_precise(precise, sl, h, h2);
			if (precise[0] && (f2 = open(precise, O_RDONLY))!=-1) {
				//there's an exact match
				//first, discard the generic one
				if (d2)
					free(d2);
				if (h2)
					free_http_headers(h2);
				close(f);
				if (fetch_http(f2, b2, CHUNK, &sl2, &h2, &d2, &dl2, &r2, 1)) {
					fprintf(stderr, "%s: cache_precise fetch_http.\n", addr);
					close(f2);
				}
				else {
					//FIXME date
					fprintf(stderr, "%s: cache_precise hit\n", addr);
					transmit_response(addr, sl2, sock, h2, d2, dl2);
					//FIXME remaining
					close(f2);
					return;
				}
			}
			else {
				//FIXME date
				fprintf(stderr, "%s: cache_generic hit\n", addr);
				transmit_response(addr, sl2, sock, h2, d2, dl2);
				//FIXME remaining
				if (d2)
					free(d2);
				if (h2)
					free_http_headers(h2);
				close(f);
				return;
			}
		}
	}
	else
		fprintf(stderr, "%s: cache miss\n", addr);
	if (!*last_host || strcmp(*last_host, host)) {
		*last_socket = get_tcp_socket(rh, rs, 0, NULL);
		if (*last_socket == -1) {
			fprintf(stderr, "%s: unreachable %s\n", addr, host);
			write(sock, BADGW, strlen(BADGW));
			return;
		}
		else
			*last_host = strdup(host);
	}
	write(*last_socket, sl, strlen(sl));
	write(*last_socket, "\r\n", 2);
	send_http_headers(*last_socket, h);
	write(*last_socket, "\r\n", 2);
	write(*last_socket, d, dl);
	//FIXME remaining
	if (fetch_http(*last_socket, b2, CHUNK, &sl2, &h2, &d2, &dl2, &r2, 1)) {
		fprintf(stderr, "%s: out fetch_http.\n", addr);
		write(sock, BADGW, strlen(BADGW));
	}
	else {
		fprintf(stderr, "%s: generic=%s, precise=%s\n", addr, generic, precise);
		if (!strncmp(sl2, HTTP_OK, strlen(HTTP_OK)))
			cache_write(generic, precise, sl2, h2, d2, dl2);
		transmit_response(addr, sl2, sock, h2, d2, dl2);
	}
}

void handle_client(int sock, struct sockaddr_in *sa, socklen_t sal) {
	char addr[INET6_ADDRSTRLEN+6] = {0};
	BUFFER *b=NULL, *b2=NULL;
	int e; //end loop flag
	int last_socket = -1; //used if keep-alive was chosen
	char *last_host = NULL;
	//request parts
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
	//process each request
	do {
		if (e = fetch_http(sock, &b, CHUNK, &sl, &h, &d, &dl, &r, 0)) {
			if (e == 1) {
				fprintf(stderr, "%s: bad request (headers?)\n", addr);
				write(sock, BADREQ, strlen(BADREQ));
			}
			//else, the client has closed the connection.
		}
		else {
			char *host = find_http_header(h, "Host");
			char *connection = find_http_header(h, "Connection");
			fprintf(stderr, "%s > %s\n", addr, sl);
			if (host) {
				char *hc = strdup(host);
				char *rh, *rs;
				int rem;
				if (hc) {
					get_host_parts(hc, &rh, &rs);
					make_request(addr, &last_socket, &last_host, host, rh, rs, &b2, sock, sl, h, d, dl);
					free(hc);
				}
				else {
					perror("strdup");
					write(sock, BADGW, strlen(BADGW));
				}
			}
			else {
				fprintf(stderr, "%s: bad request Host\n", addr);
				write(sock, BADREQ, strlen(BADREQ));
			}
			if (connection && !strcmp(connection, "close"))
				e = 1;
		}
		if (h)
			free_http_headers(h);
		if (d)
			free(d);
	} while (!e);
	if (b)
		free(b);
	if (b2)
		free(b2);
	if (last_host)
		free(last_host);
	if (last_socket != -1)
		close(last_socket);
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
