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
 * @return -1 on error
 * @return 0 when data is populated
 * @return 1 when data is incomplete (too large)
 */
int fetch_http(const int sockfd /**<socket to read the data from*/,
		char *buf /**<receive buffer*/,
		const size_t buf_len /**<receive buffer length*/,
		char **start_line /**<the line before the headers block*/,
		HTTP_HEADER **headers /**<received headers*/,
		char **data /**<data buffer*/,
		size_t *data_len /**<data length*/,
		size_t *remaining /**<amount of data still on the network*/) {
	//FIXME duplicate headers strings to free them more easily
	int headers_len=0, nread, added_len;
	char *raw_headers=NULL, *p;
	char *soh /**<start of headers (the start line is skipped)*/;
	*data = NULL;
	*headers = NULL;
	*data_len = 0;
	do {
		if ((nread=read(sockfd, buf, buf_len-1)) == -1) {
			error("fetch_http: headers");
			goto fail;
		}
		//FIXME bug when the empty line is on a boundary
		else if (p = strstr(buf, "\r\n\r\n"))
			//we reached the end of headers
			added_len = p-buf+2;
		else
			//that's a full block
			added_len = nread;
		if (raw_headers = realloc(raw_headers, headers_len+added_len+1)) {
			buf[added_len] = 0; //add a terminator
			strcat(raw_headers, buf);
			headers_len += added_len;
		}
		else {
			perror("fetch_http: realloc\n");
			return -1;
		}
	} while (!p);
	*start_line = raw_headers;
	p += 4; //start of data
	nread += buf-p; //remaining data
	soh = strchr(raw_headers, '\r');
	*soh = 0; //terminate start_line
	if (*headers = parse_http_headers(soh+2, NULL)) {
		char *h = find_http_header(*headers, "Content-Length");
		int first = 1; //first while iteration
		if (h && *h)
			*remaining = atoi(h);
		else if (**start_line=='1' || //FIXME no_data flag
				((**start_line=='2' || **start_line=='3') &&
				!strncmp(*start_line+1, "04", 2)))
			*remaining = 0; //no data
		else
			*remaining = -1; //until the connection closes
		while (*remaining) {
			if (nread) {
				*remaining -= nread;
				*data_len += nread;
				if (*data = realloc(*data, *data_len)) {
					memcpy(*data+*data_len-nread, p, nread);
					if (first) {
						//we managed not to lose the first bytes of data.
						p = buf; //first, reset the pointer used by memcpy
						//now, let's return if it's too large.
						//TODO detect size/type
						first = 0; //do this only once
					}
					nread = 0;
				}
				else {
					perror("fetch_http: realloc\n");
					goto fail;
				}
			}
			else if ((nread=read(sockfd, buf, buf_len-1)) == -1) {
				if (*remaining == -1)
					break; //normal termination
				else {
					error("fetch_http: read");
					goto fail;
				}
			}
		}
		return 0;
	}
	else {
fail:
		if (*data) {
			free(*data);
			*data = NULL;
		}
		free(raw_headers);
		return -1;
	}
}
