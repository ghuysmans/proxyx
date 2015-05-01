#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

/**
 * Reads data from a buffer created by read_until and then
 * from a file descriptor (simply using read).
 * @return number of bytes read
 * @return -1 on error
 */
ssize_t read_buffered(int fd /**<UNIX file descriptor*/,
		char *dest /**<pre-allocated destination*/,
		size_t count /**<bytes count*/,
		BUFFER **b /**<buffer pointer*/) {
	size_t position = 0;
	if (*b) {
		//read the first from the buffer
		size_t available = (*b)->size - (*b)->position;
		if (available <= count) {
			memcpy(dest, (*b)->data, available);
			free((*b));
			*b = NULL;
			position += available;
		}
		else {
			memcpy(dest, (*b)->data, count);
			(*b)->position += count;
			position += count;
		}
	}
	while (position < count) {
		int nread = read(fd, dest+position, count-position);
		if (nread > 0)
			position += nread;
		else
			break;
	}
	return position;
}

/**
 * Makes a buffered read from fd and b until s is found.
 * @note Simply deallocate *b with free() when you don't need it anymore.
 * @return number of bytes read
 * @return -1 on error
 */
ssize_t read_until(int fd /**<UNIX file descriptor*/,
		char **dest /**<destination, destroyed (without free)*/,
		char *s /**<searched string*/,
		size_t chunk /**<chunk size, in bytes*/,
		BUFFER **b /**<buffer pointer*/) {
	size_t used=0, size=0, l=strlen(s);
	//TODO memmem only on the last added part
	if (*b) {
		used = size = (*b)->size - (*b)->position;
		*dest = malloc(size);
		if (*dest)
			memcpy(*dest, (*b)->data+(*b)->position, size);
		else
			return -1;
	}
	else
		*dest = NULL;
	while (1) { //FIXME is there a cleaner way to do this?
		if (*dest && l<=used) {
			char *p = memmem(*dest, used, s, l);
			if (p) {
				//found
				size_t dest_len = p-*dest+l+1; //+terminator
				free(*b);
				if (p == *dest+used-l)
					//no buffer is needed
					*b = NULL;
				else {
					size_t remaining = used-dest_len+1;
					*b = malloc(sizeof(BUFFER)+remaining);
					(*b)->size = remaining;
					(*b)->position = 0;
					memcpy((*b)->data, p+l, remaining);
					*dest = realloc(*dest, dest_len);
				}
				return dest_len-1;
			}
		}
		if (used == size) {
			//full
			size += chunk;
			*dest = realloc(*dest, size);
			if (!*dest)
				return -1;
		}
		else {
			int nread = read(fd, *dest+used, size-used);
			if (nread > 0)
				used += nread;
			else if (!nread)
				return used;
			else {
				free(*dest);
				*dest = NULL;
				return -1;
			}
		}
	}
}

/**
 * Flushes the given buffer then uses read() on fd until it returns -1.
 * @return number of bytes read
 * @return -1 on error
 */
ssize_t read_all(int fd /**<UNIX file descriptor, -1 means none*/,
		BUFFER **b /**<buffer pointer*/,
		size_t chunk /**<chunk size, in bytes*/,
		void (*callback)(char *buffer, size_t size) /**<callback function*/) {
	char buf[chunk];
	size_t total=0, nread;
	if (*b) {
		total = (*b)->size - (*b)->position;
		callback((*b)->data+(*b)->position, total);
		free(*b);
		*b = NULL;
	}
	if (fd != -1) {
		while ((nread=read(fd, buf, chunk)) > 0) {
			callback(buf, nread);
			total += nread;
		}
		if (nread == -1)
			return -1;
	}
	return total;
}
