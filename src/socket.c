/**
 * @file
 * Low-level sockets helpers
 */
#include "socket.h"
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>


/**
 * Creates a TCP client/server socket.
 * @return descriptor
 */
int get_tcp_socket(const char *host /**<host to connect to (name or IP)*/,
		const char *service /**<service name or port number*/,
		int is_server /**<boolean, implies host==NULL*/) {
	int sock=-1, e;
	const struct addrinfo hints = {is_server?AI_PASSIVE:0,
		AF_UNSPEC, SOCK_STREAM, 0, 0, NULL, NULL, NULL};
	struct addrinfo *res=NULL, *p;
	if (e = getaddrinfo(host, service, &hints, &res))
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(e));
	else {
		for (p=res; p; p=p->ai_next) {
			if ((sock=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				continue;
			else if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
				close(sock);
				sock = -1;
			}
			else
				break;
		}
		freeaddrinfo(res);
	}
	return sock;
}
