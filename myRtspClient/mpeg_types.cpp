#include "mpeg_types.h"
#include <string.h>

MPEG_Audio MPEG_AudioObj;

size_t MPEG_Audio::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;
	int Offset = 0;
	uint8_t * DataPointer = data;

	if(!buf || !data) return 0;
	Offset = GetFlagOffset(DataPointer);
	if(Offset < MPEG_AUDIO_RTP_HEADER_SIZE) return 0;

	memcpy(buf+CopySize, data + Offset, size - Offset);
	CopySize += size - Offset;

	return CopySize;
}

int MPEG_Audio::GetFlagOffset(const uint8_t * rtp_payload)
{
	int Offset = 0;

	if(!rtp_payload) return -1;
	Offset += MPEG_AUDIO_RTP_HEADER_SIZE;
	Offset += (rtp_payload[2] << 8) + rtp_payload[3];
	return Offset;
}
