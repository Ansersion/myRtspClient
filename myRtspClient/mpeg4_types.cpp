//   Copyright 2018 Ansersion
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

#include "mpeg4_types.h"
#include <string.h>

Mpeg4Type Mpeg4TypeObj;

size_t Mpeg4Type::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;
	int Offset = 0;
	uint8_t * DataPointer = data;

	if(!buf || !data) return 0;
	Offset = GetFlagOffset(DataPointer);

	memcpy(buf+CopySize, data + Offset, size - Offset);
	CopySize += size - Offset;

	return CopySize;
}

int Mpeg4Type::GetFlagOffset(const uint8_t * rtp_payload)
{
	return 0;
}
