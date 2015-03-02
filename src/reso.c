#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#define EASYGET "GET / HTTP/1.1\r\n" \
	"Connection: close\r\n" \
	"Host: www.google.fr\r\n\r\n"

int getTcpClient(char *host, char *service) {
	int sock=-1, e;
	const struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM,
		0, 0, NULL, NULL, NULL};
	struct addrinfo *res=NULL, *p;
	if (e = getaddrinfo(host, service, &hints, &res))
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(e));
	for (p=res; p; p=p->ai_next) {
		char ip[40];
		int h = getnameinfo(p->ai_addr, p->ai_addrlen, ip, sizeof(ip),
				NULL, 0, NI_NUMERICHOST);
		if (h) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(h));
			*ip = 0; //not fatal
		}
		if ((sock=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}
		fprintf(stderr, "trying %s... ", *ip ? ip : "(?)");
		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
			perror("connect");
			close(sock);
			sock = -1;
		}
		else
			break;
	}
	if (sock == -1)
		fprintf(stderr, "couldn't connect to %s\n", host);
	else {
		fprintf(stderr, "connected\n");
		freeaddrinfo(res);
	}
	return sock;
}

int main(int argc, char *argv[]) {
	char *prot="http";
	int o, verbose=0;
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
	if (argv[optind]) {
		int sock;
		ssize_t r;
		char buf[1024];
		if ((sock=getTcpClient(argv[optind], prot)) == -1)
			return 1;
		if (send(sock, EASYGET, strlen(EASYGET), 0) == -1)
			error(errno, errno, "send");
		while ((r = recv(sock, buf, sizeof(buf), 0))!=0 && r!=-1) {
			buf[r] = 0;
			puts(buf);
		}
		if (r == -1)
			perror("recv");
		close(sock);
	}
	else
		printf("Usage: %s name\n", argv[0]);
	return 0;
}
