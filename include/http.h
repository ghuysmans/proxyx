#ifndef _HTTP_H
#define _HTTP_H

#include "http_headers.h"
#include "buffer.h"

int fetch_http(const int sockfd, BUFFER **b, size_t chunk, char **start_line, HTTP_HEADER **headers, char **data, size_t *data_len, ssize_t *remaining, int force_data);

#endif //_HTTP_H
