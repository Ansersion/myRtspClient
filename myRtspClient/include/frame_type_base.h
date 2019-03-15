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

#ifndef FRAME_TYPE_BASE_H
#define FRAME_TYPE_BASE_H

#include <string>
#include <stdint.h>
#include "sdp_data.h"
// #include "nalu_types.h"

using std::string;

class FrameTypeBase
{
    public:
        static FrameTypeBase * CreateNewFrameType(string & EncodeType);
        static void DestroyFrameType(FrameTypeBase * frameTypeBase);

	public:
		FrameTypeBase() {};
		virtual ~FrameTypeBase() {};

	public:
        virtual void Init() {}
        virtual uint8_t * PrefixParameterOnce(uint8_t * buf, size_t * size) {return buf;}
        virtual bool NeedPrefixParameterOnce() { return false; }
        virtual uint8_t * SuffixParameterOnce(uint8_t * buf, size_t * size) {return buf;}
        virtual bool NeedSuffixParameterOnce() { return false; }
        virtual int PrefixParameterEveryFrame() {return 0;}
        virtual int PrefixParameterEveryPacket() {return 0;}
        virtual int SuffixParameterOnce() {return 0;}
        virtual int SuffixParameterEveryFrame() {return 0;}
        virtual int SuffixParameterEveryPacket() {return 0;}
	 	virtual int ParsePacket(const uint8_t * RTPPayload, size_t size, bool * EndFlag) {
            if(EndFlag) *EndFlag = true;
            return 0;
        }
		virtual int ParseFrame(const uint8_t * RTPPayload) {return 0;}

        virtual int ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo) {return 0;}

		/* To play the media sessions 
		 * return: 
		 * 	0: not a complete frame, which means there are more packets; other: a complete frame
		 * */
		virtual bool IsFrameComplete(const uint8_t * RTPPayload) {return true;}
		// virtual int AssemblePacket(const uint8_t * RTPPayload) {return 0;}
		// virtual int GetFlagOffset(const uint8_t * RTPPayload) {return 0;}
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) {return 0;}


};

#endif
