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

#include "mpeg_types.h"
#include <string.h>

// MPEG_Audio MPEG_AudioObj;

// size_t MPEG_Audio::CopyData(uint8_t * buf, uint8_t * data, size_t size)
// {
// 	size_t CopySize = 0;
// 	int Offset = 0;
// 	uint8_t * DataPointer = data;
// 
// 	if(!buf || !data) return 0;
// 	Offset = GetFlagOffset(DataPointer);
// 	if(Offset < MPEG_AUDIO_RTP_HEADER_SIZE) return 0;
// 
// 	memcpy(buf+CopySize, data + Offset, size - Offset);
// 	CopySize += size - Offset;
// 
// 	return CopySize;
// }
// 
// int MPEG_Audio::GetFlagOffset(const uint8_t * rtp_payload)
// {
// 	int Offset = 0;
// 
// 	if(!rtp_payload) return -1;
// 	Offset += MPEG_AUDIO_RTP_HEADER_SIZE;
// 	Offset += (rtp_payload[2] << 8) + rtp_payload[3];
// 	return Offset;
// }
const string MPEG_Audio::ENCODE_TYPE = "MPA";

size_t MPEG_Audio::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;
	// int Offset = 0;
	// uint8_t * DataPointer = data;

	if(!buf || !data) return 0;
	if(Offset < MPEG_AUDIO_RTP_HEADER_SIZE) return 0;
	if(size < Offset) return 0;

	memcpy(buf+CopySize, data + Offset, size - Offset);
	CopySize += size - Offset;

	return CopySize;
}

int MPEG_Audio::ParsePacket(const uint8_t * packet, size_t size, bool * EndFlagTmp)
{
 	Offset = 0;
    if(!packet || !EndFlagTmp) {
        return -1;
    }
    if(size < MPEG_AUDIO_RTP_HEADER_SIZE) {
        return -2;
    }
 	Offset += MPEG_AUDIO_MBZ_SIZE;
 	Offset += MPEG_AUDIO_FLAG_OFFSET_SIZE;
 	Offset += (packet[2] << 8) + packet[3];
    *EndFlagTmp = true;
    return 0;
}
