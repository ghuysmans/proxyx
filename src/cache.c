/**
 * @file
 * Cache management functions
 */
#include "cache.h"
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GET_METHOD "GET "

/**
 * Determines whether the Expires field is in the past.
 * @return 2 on error
 */
int expired(const HTTP_HEADER *h) {
	char *e = find_http_header(h, "Expires");
	struct tm tm;
	if (e && !from_http_date(e, &tm)) {
		time_t tt = mktime(&tm);
		return time(NULL) >= tt;
	}
	else
		return 2;
}

/**
 * Writes data to the cache if headers allow it.
 */
void cache_write(const char *generic /**<hash*/,
		const char *precise /**<hash*/,
		const char *sl2 /**<client's start line*/,
		const HTTP_HEADER *h2 /**<client's first header*/,
		const char *d2 /**client's data*/,
		const size_t dl2 /**client's data length*/) {
	//FIXME conditions
	if (*precise || *generic) {
		char *cc = find_http_header(h2, "Cache-Control");
		char *p = find_http_header(h2, "Pragma");
		if (cc && (strstr(cc, "private") || strstr(cc, "must-revalidate") ||
				strstr(cc, "no-cache") || strstr(cc, "no-store")) ||
				p && !strcmp(p, "no-cache"))
			;
		else {
			int fd;
			fd = creat(*precise ? precise : generic, S_IRUSR);
			if (fd == -1)
				perror("creat");
			else {
				write(fd, sl2, strlen(sl2));
				write(fd, "\r\n", 2);
				send_http_headers(fd, h2);
				write(fd, "\r\n", 2);
				write(fd, d2, dl2);
				//FIXME remaining
				close(fd);
				if (precise)
					//create a generic link if it doesn't already exist
					link(precise, generic);
			}
		}
	}
}

/**
 * Computes a generic filename based on a start line.
 * @note Only GET requests have one (because others can't be cached).
 */
void cache_generic(char *dest /**<pre-allocated buffer*/,
		const char *sl /**<start line*/) {
	if (strncmp(sl, GET_METHOD, strlen(GET_METHOD)))
		*dest = 0;
	else {
		unsigned char hash[MD5_DIGEST_LENGTH];
		int i;
		MD5_CTX ctx;
		MD5_Init(&ctx);
		MD5_Update(&ctx, sl, strlen(sl));
		MD5_Final(hash, &ctx);
		for (i=0; i<MD5_DIGEST_LENGTH; i++)
			snprintf(dest+(i<<1), 3, "%02x", hash[i]);
	}
}

/**
 * Computes a precise filename based on a request and a response.
 */
void cache_precise(char *dest /**<pre-allocated buffer*/,
		const char *sl /**<start line*/,
		const HTTP_HEADER *h /**<client's first HTTP header*/,
		const HTTP_HEADER *h2 /**<server's first HTTP header*/) {
	unsigned char hash[MD5_DIGEST_LENGTH];
	char *vary = find_http_header(h2, "Vary");
	int i;
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, sl, strlen(sl));
	if (vary) {
		char *v;
		vary = strdup(vary);
		//FIXME error handling?
		for (v=strtok(vary, ", "); v; v=strtok(NULL, ", ")) {
			if (*v == '*') {
nope:
				*dest = 0;
				//FIXME no memory leak from MD5_Init?
				return;
			}
			else {
				char *d = find_http_header(h, v);
				if (d)
					MD5_Update(&ctx, d, strlen(d));
			}
		}
		free(vary);
		MD5_Final(hash, &ctx);
		for (i=0; i<MD5_DIGEST_LENGTH; i++)
			snprintf(dest+(i<<1), 3, "%02x", hash[i]);
	}
	else
		goto nope;
}
