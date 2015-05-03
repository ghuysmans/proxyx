/**
 * @file
 * HTTP library
 */
#include "http.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Fetches data using HTTP.
 * @return 1 on error
 * @return 2 if the connection was closed
 */
int fetch_http(const int sockfd /**<socket to read the data from*/,
		BUFFER **b /**<buffer pointer*/,
		size_t chunk /**<chunk size, in bytes*/,
		char **start_line /**<the line before the headers block*/,
		HTTP_HEADER **headers /**<received headers*/,
		char **data /**<data buffer*/,
		size_t *data_len /**<data length*/,
		ssize_t *remaining /**<remaining data length, -1 means unknown*/,
		int force_data /**<expect data even without Content-Length*/) {
	//FIXME duplicate headers strings to free them more easily
	int sll = read_until(sockfd, start_line, "\r\n", chunk, b);
	*headers = NULL;
	*data = NULL;
	if (sll < 2) {
nope:
		if (*start_line) {
			free(*start_line);
			*start_line = NULL;
		}
		return sll ? 1 : 2;
	}
	else {
		char *rh; //raw headers
		char *cl; //Content-Length header value
		size_t headers_len, full_length;
		(*start_line)[sll-2] = 0; //terminator
		headers_len = read_until(sockfd, &rh, "\r\n\r\n", chunk, b);
		if (headers_len < 4) {
			//no headers
			if (rh)
				free(rh);
			if (headers_len == -1)
				goto nope;
		}
		else {
			rh[headers_len-2] = 0; //terminator
			*headers = parse_http_headers(rh, NULL);
		}
		cl = find_http_header(*headers, "Content-Length");
		if (cl) {
			full_length = atoi(cl);
			if (full_length > chunk)
				*remaining = full_length-chunk;
			else
				*remaining = 0;
			*data_len = full_length-*remaining;
			if (*data_len) {
				if (*data = malloc(*data_len+1)) {
					int nread = read_buffered(sockfd, *data, *data_len, b);
					(*data)[nread] = 0;
					return nread != *data_len;
				}
				else
					return 1;
			}
			else
				//empty
				*data = NULL;
		}
		else if (!strncmp(*start_line, "HTTP/1.1 1", 10) ||
				!strncmp(*start_line, "HTTP/1.1 204", 12) ||
				!strncmp(*start_line, "HTTP/1.1 304", 12) ||
				!force_data) {
			*data_len = *remaining = 0;
			*data = NULL;
		}
		else {
			*remaining = -1; //unknown
			if (*data = malloc(chunk+1)) {
				*data_len = read_buffered(sockfd, *data, chunk, b);
				if (*data_len < chunk)
					*remaining = 0;
				(*data)[*data_len] = 0;
			}
			else
				return 1;
		}
		return 0;
	}
}
