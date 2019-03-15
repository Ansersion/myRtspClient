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

#ifndef PCMU_TYPES_H
#define PCMU_TYPES_H

#include <string>
#include <stdint.h>
#include <iostream>
#include <frame_type_base.h>

using std::string;

class PCMUTypeBase : public FrameTypeBase
{
	public:
		virtual ~PCMUTypeBase() {};

	public:
        virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) { return FrameTypeBase::CopyData(buf, data, size);}
        virtual int ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo) { return FrameTypeBase::ParseParaFromSDP(sdpMediaInfo);}
        virtual int ParsePacket(const uint8_t * packet, size_t size, bool * EndFlagTmp) {return FrameTypeBase::ParsePacket(packet, size, EndFlagTmp);}
		// virtual int GetFlagOffset(const uint8_t * RTPPayload) { return -1; };
};


class PCMU_Audio : public PCMUTypeBase
{
	public:
        static const string ENCODE_TYPE;

		virtual ~PCMU_Audio() {};
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
};

#endif
