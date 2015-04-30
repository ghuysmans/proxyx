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
ssize_t read_all(int fd, BUFFER **b, size_t chunk, void (*callback)(char *buffer, size_t size));

#endif //_BUFFER_H
