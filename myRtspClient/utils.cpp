//   Copyright 2015-2017 Ansersion
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#include "utils.h"
#include "md5.h"
#include <string.h>
#include <stdio.h>

ssize_t	ReadLine(int fd, void *vptr, size_t maxlen)
{   
	size_t n; 
	ssize_t rc;
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

int Md5sum32(void * input, unsigned char * output, size_t input_size, size_t output_size)
{
	unsigned char decrypt[16];
	unsigned char decrypt_ascii[33];
	if(!input) {
		return -1;
	}	
	if(!output) {
		return -1;
	}
	if(output_size < 32) {
		return -1;
	}
	MD5_CTX md5;  
	MD5Init(&md5);                
	MD5Update(&md5,(unsigned char *)input,input_size);  
	MD5Final(&md5,decrypt);          
	for(int i = 0; i < 16; i++) {
		sprintf((char *)&decrypt_ascii[i*2], "%02x",decrypt[i]);  
	}
	decrypt_ascii[32] = '\0';
	memcpy(output, decrypt_ascii, 32);
	return 0;
}
