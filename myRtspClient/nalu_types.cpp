//   Copyright 2019 Ansersion
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

#include "nalu_types.h"
#include "Base64.hh"
#include <string.h>

uint8_t * NALUTypeBase::PrefixXPS(uint8_t * buf, size_t * size, string & xps)
{
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	*size += 4;

	unsigned int XpsSize = 0;
	unsigned char * xpsBuf = base64Decode(xps.c_str(), XpsSize, true);
    if(!xpsBuf) return NULL;
	memcpy(buf + (*size), xpsBuf, XpsSize);
	*size += XpsSize;
	delete[] xpsBuf;
	xpsBuf = NULL;

	return buf;
}
