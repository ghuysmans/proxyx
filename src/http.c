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
 */
int fetch_http(const int sockfd /**<socket to read the data from*/,
		BUFFER **b /**<buffer pointer*/,
		size_t chunk /**<chunk size, in bytes*/,
		char **start_line /**<the line before the headers block*/,
		HTTP_HEADER **headers /**<received headers*/,
		char **data /**<data buffer*/,
		size_t *data_len /**<data length*/,
		size_t *remaining /**<remaining data length*/) {
	//FIXME duplicate headers strings to free them more easily
	int sll = read_until(sockfd, start_line, "\r\n", chunk, b);
	if (sll == -1)
		return 1;
	else {
		char *rh;
		int headers_len;
		if (sll < 2) {
nope:
			free(*start_line);
			*start_line = NULL;
			return 1;
		}
		else {
			(*start_line)[sll-2] = 0; //terminator
			headers_len = read_until(sockfd, &rh, "\r\n\r\n", chunk, b);
			if (headers_len == -1)
				goto nope;
			else {
				//terminator
				//FIXME does it even work?
				if (headers_len < 2)
					rh[headers_len] = 0;
				else
					rh[headers_len-2] = 0;
				if (*headers = parse_http_headers(rh, NULL)) {
					char *h = find_http_header(*headers, "Content-Length");
					size_t full_length;
					if (h && *h) {
						full_length = atoi(h);
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
						else {
							*data = NULL;
							return 0;
						}
					}
					else if (!strncmp(*start_line, "HTTP/1.1 1", 10) ||
							!strncmp(*start_line, "HTTP/1.1 204", 12) ||
							!strncmp(*start_line, "HTTP/1.1 304", 12)) {
						*data_len = *remaining = 0;
						*data = NULL;
					}
					else {
						*remaining = -1; //unknown
						if (*data = malloc(*data_len+1)) {
							*data_len = read_buffered(sockfd, *data, chunk, b);
							if (*data_len < chunk)
								*remaining = 0;
							(*data)[*data_len] = 0;
							return 0;
						}
						else
							return 1;
					}
				}
			}
		}
	}
}
