#ifndef _HTTP_H
#define _HTTP_H

#include "http_headers.h"

int fetch_http(const char *host, const char *service, const char *method, const char *protocol, const HTTP_HEADER *request_headers, HTTP_HEADER **response_headers, char **data, size_t *data_len);
void handle_http_client(int sockfd);
void process_http_response(int usersock, HTTP_HEADER *headers, char *data, size_t *data_len);

#endif //_HTTP_H
