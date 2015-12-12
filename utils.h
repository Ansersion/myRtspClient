#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <errno.h>

/* Read line by line.
 * See also:
 * <<Unix Network Programming>>*/
ssize_t ReadLine(int fd, void * buf, size_t maxlen);

/* Read n bytes
 * See also:
 * <<Unix Network Programming>>*/
ssize_t Readn(int fd, void *vptr, size_t n);

/* Write n bytes.
 * See also:
 * <<Unix Network Programming>>*/
ssize_t Writen(int fd, const void *vptr, size_t n);

#endif
