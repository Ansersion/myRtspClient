//   Copyright 2015-2019 Ansersion
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
/*******************************/
/* More info refer to RFC 7655 */
/*******************************/


#include "pcmu_types.h"
#include <string.h>

PCMU_Audio PCMU_AudioObj;

const string PCMU_Audio::ENCODE_TYPE = "PCMU";

size_t PCMU_Audio::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;

	if(!buf || !data) return 0;
	memcpy(buf + CopySize, data, size);
	CopySize += size;

	return CopySize;
}
