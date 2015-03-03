/**
 * @file
 * HTTP library
 */
#include "http.h"

/**
 * Fetches data using HTTP.
 * @return 0 on success
 */
int fetch_http(const char *host /**<host to connect to*/,
		const char *service /**<service or port number*/,
		const char *method /**<HTTP method*/,
		const char *protocol /**<protocol in the HTTP request*/,
		const HTTP_HEADER *request_headers /**<sent headers*/,
		HTTP_HEADER **response_headers /**<received headers*/,
		char **data /**<received data*/,
		size_t *data_len /**<received data length*/);

/**
 * Handles an HTTP client, first accepting an incoming connection.
 */
void handle_http_client(int sockfd /**<listening socket*/);

/**
 * Processes an HTTP response (for scripting, etc.).
 */
void process_http_response(int usersock /**<user socket*/,
		HTTP_HEADER *headers /**<HTTP headers*/,
		char *data /**<Response data*/,
		size_t *data_len /**<Response data length*/);
