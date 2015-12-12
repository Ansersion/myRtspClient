#include "utils.h"

ssize_t	ReadLine(int fd, void *vptr, size_t maxlen)
{   
	ssize_t n, rc;
	char c, *ptr;

	ptr = (char *)vptr; 
	for(n = 1; n < maxlen; n++) {
again:
		if((rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if(c == '\n') 
				break;
		} else if(rc == 0) {
			*ptr = 0;
			return n - 1;
		} else {
			if(errno == EINTR)
				goto again;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) 
				return n - 1;
			return -1;
		}
	}
	*ptr = 0;
	return n;
}

ssize_t Writen(int fd, const void *vptr, size_t n)
{
	size_t      nleft;
	ssize_t     nwritten;
	const char  *ptr;

	ptr = (const char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;       /* and call write() again */
			else if(nwritten < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) 
				return (n - nleft);
			else
				return(-1);         /* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

ssize_t Readn(int fd, void *vptr, size_t n)
{
	size_t  nleft;
	ssize_t nread;
	char    *ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;      /* and call read() again */
			else if(errno == EWOULDBLOCK || errno == EAGAIN)
				return (n - nleft);
			else
				return(-1);
		} else if (nread == 0)
			break;              /* EOF */

		nleft -= nread;
		ptr   += nread;
	}   
	return(n - nleft);      /* return >= 0 */
}
/* end readn */
