//   Copyright 2015-2018 Ansersion
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

#include "pcmu_types.h"
#include <string.h>

PCMU_Audio PCMU_AudioObj;

size_t PCMU_Audio::CopyData(uint8_t * buf, uint8_t * data, size_t size)
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

int PCMU_Audio::GetFlagOffset(const uint8_t * rtp_payload)
{
	return 0;
}
