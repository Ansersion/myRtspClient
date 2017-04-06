//   Copyright 2015 Ansersion
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

#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <errno.h>

#define MD5_SIZE 	32
#define MD5_BUF_SIZE 	(MD5_SIZE + sizeof('\0'))

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

/*
param: 
output: must sizeof(output) >= 32
   */
int Md5sum32(void * input, unsigned char * output, size_t input_size, size_t output_size);

#endif
