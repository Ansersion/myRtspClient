//   Copyright 2015-2016 Ansersion
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
/* More info refer to RFC 6184 */
/*******************************/

#ifndef NALU_TYPES_H
#define NALU_TYPES_H

#include <string>
#include <stdint.h>
#include <frame_type_base.h>

#define NAL_UNIT_TYPE_NUM_H264          32
#define PACKETIZATION_MODE_NUM_H264     3

#define NAL_UNIT_TYPE_NUM_H265          64

#define PACKET_MODE_SINGAL_NAL          0
#define PACKET_MODE_NON_INTERLEAVED     1
#define PACKET_MODE_INTERLEAVED         2

#define IS_PACKET_MODE_VALID_H264(P) 	\
	((P) >= PACKET_MODE_SINGAL_NAL && (P) <= PACKET_MODE_INTERLEAVED)

/* More info refer to H264 'nal_unit_type' */

class NALUTypeBase : public FrameTypeBase
{
	public:
		// NALU types map for h264 
		static NALUTypeBase * NalUnitType_H264[PACKETIZATION_MODE_NUM_H264][NAL_UNIT_TYPE_NUM_H264];
		// NALU types map for h265 
		static NALUTypeBase * NalUnitType_H265[1][NAL_UNIT_TYPE_NUM_H265];
	public:
		virtual ~NALUTypeBase() {};
	public:
		virtual uint16_t ParseNALUHeader_F(const uint8_t * RTPPayload) = 0;
		virtual uint16_t ParseNALUHeader_Type(const uint8_t * RTPPayload) = 0;

		virtual uint16_t ParseNALUHeader_NRI(const uint8_t * RTPPayload) {return 0;} // only for h264
		virtual uint16_t ParseNALUHeader_Layer_ID(const uint8_t * RTPPayload) {return 0;} // only for h265
		virtual uint16_t ParseNALUHeader_Temp_ID_Plus_1(const uint8_t * RTPPayload) {return 0;} // only for h265
		
		virtual bool IsPacketStart(const uint8_t * rtp_payload) = 0;
		virtual bool IsPacketEnd(const uint8_t * rtp_payload) = 0;
		virtual bool IsPacketReserved(const uint8_t * rtp_payload) { return false;}
		virtual bool IsPacketThisType(const uint8_t * rtp_payload) = 0;
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) = 0;
		virtual NALUTypeBase * GetNaluRtpType(int packetization, int nalu_type_id) = 0;
		virtual std::string GetName() const { return Name; }
		virtual bool GetEndFlag() { return EndFlag; }
		virtual bool GetStartFlag() { return StartFlag; }
	protected:
		std::string Name;
		bool EndFlag;
		bool StartFlag;
};

#endif
