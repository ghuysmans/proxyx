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

int fetch_response(int socket, char** raw_headers, char** raw_content){
	int nread, len;//nread seems to be the number of bytes we already read, len seems to be the length of what we got so far for the header/content
	int headers_len=0, content_len=0;
	char buf[BUFFER_LEN], *p;
	//init for realloc
	*raw_headers = NULL;
	*raw_content = NULL;

	//headers
	//here, p is the end of the headers part
	do {
		nread=read(socket, buf, BUFFER_LEN-1);
		printf("Read %d bytes\n", nread);
		if (nread == -1) {
			free(*raw_headers);
			error("Header reading error");
			return 2;
		}
		if (p = strstr(buf, "\r\n\r\n")){//If we reached the end (p then is a pointer to the beggining of the string)
			len = p-buf;//p-buf+2
			printf("current header piece len : %d\n",len);
		}
		else
			len = nread;
		*raw_headers = realloc(*raw_headers, headers_len+len+1);
		if (*raw_headers == NULL)
			return 1;
		buf[len] = 0;//0 terminal
		strcat(*raw_headers, buf);//Add in raw_headers what we parsed so far
		headers_len += len;
	} while (!p);

	//content
	do {
		if (p){
			len = &buf[nread]- p - 4;//Everything minus the header and \r\n\r\n
			printf("current content piece len : %d\n",len);
		}
		else {//Error with the header
			len = read(socket, buf, BUFFER_LEN-1);
			if (len == -1) {
				free(*raw_headers);
				free(*raw_content);
				error("Reading error");
				return 2;
			}
		}
		*raw_content = realloc(*raw_content, content_len+len+1);
		if (*raw_content == NULL)
			return 1;
		//LOL NOT WORKING WITH BINARY CONTENT, U MAD BRO ???
		//buf[nread] = 0;
		//strcat(*raw_content, p+4);//we skip the ending '\r\n\r\n' and we start right after that
		memcpy(*raw_content, p+4, nread-headers_len-4);
		content_len += len;
	} while (0);
	return 0;
}
