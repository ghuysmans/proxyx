#ifndef _HTTP_HEADERS
#define _HTTP_HEADERS

#include <time.h>

#ifndef _HTTP_HEADER_STRUCT
#define _HTTP_HEADER_STRUCT
typedef struct _HTTP_HEADER {
	char *field_name;
	char *field_value;
	struct _HTTP_HEADER *next;
} HTTP_HEADER;
#endif //_HTTP_HEADER_STRUCT

HTTP_HEADER *parse_http_headers(char *raw, HTTP_HEADER *nx);
void free_http_headers(const HTTP_HEADER *h);
char *find_http_header(const HTTP_HEADER *h, const char *name);
HTTP_HEADER *add_http_header(const HTTP_HEADER *h, const char *name, const char *value);
void send_http_headers(const int fd, const HTTP_HEADER *h);
int from_http_date(const char *raw, struct tm *d);
char *to_http_date(const struct tm *d);
void get_host_parts(const char *field_value, char **host, char **service);

#endif //_HTTP_HEADERS
