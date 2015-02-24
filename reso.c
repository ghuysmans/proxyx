#include <error.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

struct in_addr resolve(char *name) {
	struct in_addr in;
	unsigned long l;
	struct hostent *ent;
	if ((l = inet_addr(name)) != INADDR_NONE) {
		//IP address
		in.s_addr = l;
		return in;
	}
	else if (! (ent=gethostbyname(name))) {
		//unknown host
		in.s_addr = INADDR_NONE;
		return in;
	}
	else
		//resolved DNS (we got an IP)
		return *(struct in_addr*)ent->h_addr;
}

int main(int argc, char *argv[]) {
	if (argc == 2) {
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		struct in_addr ip = resolve(argv[1]);
		struct sockaddr_in target = {AF_INET, htons(1111), ip, 0};
		printf("IP: %s\n", inet_ntoa(ip));
		if (sock == -1)
			error(errno, errno, "socket");
		else {
			if (connect(sock, (struct sockaddr*)&target, sizeof(target)) == -1)
				error(errno, errno, "connect");
			printf("connected\n");
			if (send(sock, "pouet\n", 6, 0) == -1)
				error(errno, errno, "send");
			shutdown(sock, SHUT_RDWR);
		}
	}
	else
		printf("Usage: %s name\n", argv[0]);
	return 0;
}
