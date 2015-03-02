/**
 * @file
 * Low-level sockets helpers
 */
#include "socket.h"

/**
 * Creates a TCP socket.
 * @return descriptor
 */
int get_tcp_socket(const char *host /**<host to connect/listen to*/,
		const char *service /**<service or port number*/,
		int is_server /**<boolean*/);
