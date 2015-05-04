#ifndef _CACHE_H
#define _CACHE_H

#include "http_headers.h"
#define CACHE_FILENAME_LENGTH 33

void cache_write(const char *generic, const char *precise, const char *sl2, const HTTP_HEADER *h2, const char *d2, const size_t dl2);
void cache_generic(char *dest, const char *sl);
void cache_precise(char *dest, const char *sl, const HTTP_HEADER *h, const HTTP_HEADER *h2);

#endif //_CACHE_H
