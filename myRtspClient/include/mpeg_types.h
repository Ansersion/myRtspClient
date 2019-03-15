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

/*******************************/
/* More info refer to RFC 2250 */
/*******************************/

#ifndef MPEG_TYPES_H
#define MPEG_TYPES_H

#include <string>
#include <stdint.h>
#include <frame_type_base.h>

using std::string;

#define MPEG_AUDIO_MBZ_SIZE 	2
#define MPEG_AUDIO_FLAG_OFFSET_SIZE 	2
#define MPEG_AUDIO_RTP_HEADER_SIZE 	(MPEG_AUDIO_MBZ_SIZE + MPEG_AUDIO_FLAG_OFFSET_SIZE)

class MPEGTypeBase : public FrameTypeBase
{
	public:
		// MPEGTypeBase() {Name.assign("MPEGTypeBase");};
		virtual ~MPEGTypeBase() {};

        virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) { return FrameTypeBase::CopyData(buf, data, size);}
        virtual int ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo) { return FrameTypeBase::ParseParaFromSDP(sdpMediaInfo);}
        virtual int ParsePacket(const uint8_t * packet, size_t size, bool * EndFlagTmp) {return FrameTypeBase::ParsePacket(packet, size, EndFlagTmp);}

	public:
		// virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) { return 0;};
		// virtual int GetFlagOffset(const uint8_t * RTPPayload) { return -1; };

	// protected:
	// 	std::string Name;
};

class MPEG_Audio : public MPEGTypeBase
{
	public:
        static const string ENCODE_TYPE;
		virtual ~MPEG_Audio() {};
		
	public:
		// virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
		// virtual int GetFlagOffset(const uint8_t * RTPPayload);
	 	virtual int ParsePacket(const uint8_t * RTPPayload, size_t size, bool * EndFlag);
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);

    private:
        size_t Offset;
};

#endif
