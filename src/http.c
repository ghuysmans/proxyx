/**
 * @file
 * HTTP library
 */
#include "http.h"

/**
 * Fetches data using HTTP.
 * @return 0 when data is populated
 * @return 1 when data is incomplete (too large).
 */
int fetch_http(const int sockfd /**<socket to read the data from*/,
		char *receive /**<receive buffer*/,
		const size_t receive_len /**<receive buffer length*/,
		char **start_line /**<the line before the headers block*/,
		HTTP_HEADER **headers /**<received headers*/,
		char **data /**<data buffer*/,
		size_t *data_len /**<data length*/);

/**
 * Handles an HTTP client.
 */
void handle_http_client(int sockfd /**<socket returned by accept()*/);
