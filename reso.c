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
	if (argc == 2)
		printf("%s\n", inet_ntoa(resolve(argv[1])));
	else
		printf("Usage: %s name\n", argv[0]);
	return 0;
}
