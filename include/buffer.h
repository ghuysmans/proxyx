#ifndef _BUFFER_H
#define _BUFFER_H

#include <unistd.h>

typedef struct {
	size_t size;
	size_t position;
	char data[];
} BUFFER;

ssize_t read_buffered(int fd, char *dest, size_t count, BUFFER **b);
ssize_t read_until(int fd, char **dest, char *s, size_t chunk, BUFFER **b);

#endif //_BUFFER_H
