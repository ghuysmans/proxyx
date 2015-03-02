/**
 * @file
 * HTTP library
 */
#include "http.h"

/**
 * Handles an HTTP client.
 */
void handle_http_client(int sockfd, /**<listening socket*/
		const struct sockaddr *addr, /**<network address*/
		socklen_t addr_len /**<network address length*/);
