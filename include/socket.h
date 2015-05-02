#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>

int get_tcp_socket(const char *host, const char *service, int server_backlog, socklen_t *addrlen);

#endif //_SOCKET_H
