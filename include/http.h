#ifndef _HTTP_H
#define _HTTP_H

#include "http_headers.h"

int fetch_http(const int sockfd, char *buf, const size_t buf_len, char **start_line, HTTP_HEADER **headers, char **data, size_t *data_len, size_t *remaining);
void handle_http_client(int sockfd);

#endif //_HTTP_H
