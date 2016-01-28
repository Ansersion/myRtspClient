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

#ifndef NALU_TYPES_H265_H
#define NALU_TYPES_H265_H

#include <string>
#include <stdint.h>

/* More info refer to H265 'nal_unit_type' */
#define IS_NALU_TYPE_VALID_H265(N) 		\
	( \
      ((N) >= 0 && (N) <= 40) || \
      ((N) == APs_H265::APs_ID_H265) || \
      ((N) == FUs_H265::FUs_ID_H265) \
	)

class NALUTypeBase_H265
{
	public:
		NALUTypeBase_H265() { Name.assign("TypeBase_H265"); }
		virtual ~NALUTypeBase_H265() {};
	public:
		virtual uint16_t ParseNALUHeader_F(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Type(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Layer_ID(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Temp_ID_Plus_1(const uint8_t * RTPPayload);
		virtual bool IsPacketStart(const uint8_t * rtp_payload) {return true; }
		virtual bool IsPacketEnd(const uint8_t * rtp_payload) { return true; }
		virtual bool IsPacketThisType(const uint8_t * rtp_payload);
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
		virtual std::string GetName() const { return Name; }
		virtual bool GetEndFlag() { return EndFlag; }
		virtual bool GetStartFlag() { return StartFlag; }
	protected:
		std::string Name;
		bool EndFlag;
		bool StartFlag;
};

class APs_H265 : public NALUTypeBase_H265
{
	public:
		APs_H265() { Name.assign("APs_H265"); };
		virtual ~APs_H265() {};

	public:
		static const uint16_t APs_ID_H265;
};

class FUs_H265 : public NALUTypeBase_H265
{
	public:
		FUs_H265() { Name.assign("FUs_H265"); };
		virtual ~FUs_H265() {};
	public:
		/* Function: "ParseNALUHeader_*":
		 * 	Return 'FU_A_ERR'(0xFF) if error occurred */
		uint16_t ParseNALUHeader_F(const uint8_t * rtp_payload);
		uint16_t ParseNALUHeader_NRI(const uint8_t * rtp_payload);
		uint16_t ParseNALUHeader_Type(const uint8_t * rtp_payload);
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
	public:
		static const uint16_t FUs_ID_H265;

	public:
		/* if FU_A payload type */
		bool IsPacketThisType(const uint8_t * rtp_payload);

		/* Packet Start Flag */
		bool IsPacketStart(const uint8_t * rtp_payload);

		/* Packet End Flag */
		bool IsPacketEnd(const uint8_t * rtp_payload);

		/* Reserved */
		bool IsPacketReserved(const uint8_t * rtp_payload);

};

#endif
