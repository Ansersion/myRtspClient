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
/* More info refer to RFC 6184 */
/*******************************/

#ifndef NALU_TYPES_H
#define NALU_TYPES_H

#include <string>
#include <stdint.h>

#define NAL_UNIT_TYPE_NUM 		32
#define PACKETIZATION_MODE_NUM 	3

#define PACKET_MODE_SINGAL_NAL 			0
#define PACKET_MODE_NON_INTERLEAVED 	1
#define PACKET_MODE_INTERLEAVED 		2

#define IS_PACKET_MODE_VALID(P) 	\
	((P) >= PACKET_MODE_SINGAL_NAL && (P) <= PACKET_MODE_INTERLEAVED)

/* More info refer to H264 'nal_unit_type' */
#define IS_NALU_TYPE_VALID(N) 		\
	( \
      ((N) >= 1 && (N) <= 12) || \
      ((N) == STAP_A::STAP_A_ID) || \
      ((N) == STAP_B::STAP_B_ID) || \
      ((N) == MTAP_16::MTAP_16_ID) || \
      ((N) == MTAP_24::MTAP_24_ID) || \
      ((N) == FU_A::FU_A_ID) || \
      ((N) == FU_B::FU_B_ID) \
	)

class NALUTypeBase
{
	public:
		static NALUTypeBase * NalUnitType[PACKETIZATION_MODE_NUM][NAL_UNIT_TYPE_NUM];
	public:
		NALUTypeBase() {Name.assign("BaseType");};
		virtual ~NALUTypeBase() {};
	public:
		virtual uint8_t ParseNALUHeader_F(const uint8_t * RTPPayload);
		virtual uint8_t ParseNALUHeader_NRI(const uint8_t * RTPPayload);
		virtual uint8_t ParseNALUHeader_Type(const uint8_t * RTPPayload);
		virtual bool IsPacketStart(const uint8_t * rtp_payload) {return true; }
		virtual bool IsPacketEnd(const uint8_t * rtp_payload) { return true; }
		virtual bool IsPacketReserved(const uint8_t * rtp_payload) { return false; }
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

// class NonRTPPacketType : public NALUTypeBase
// {
// 	public: 
// 		NonRTPPacketType() { Name.assign("NonRTPPacketType"); };
// 		virtual ~NonRTPPacketType() {};
// 	public:
// 		uint8_t ParseNALUHeader_F(const uint8_t * RTPPayload) {return 0;};
// 		uint8_t ParseNALUHeader_NRI(const uint8_t * RTPPayload) {return 0;};
// 		uint8_t ParseNALUHeader_Type(const uint8_t * RTPPayload) {return 0;};
// 		bool IsPacketStart(const uint8_t * rtp_payload) {return true;};
// 		bool IsPacketEnd(const uint8_t * rtp_payload) {return true;};
// 		bool IsPacketReserved(const uint8_t * rtp_payload) {return false;};
// 		bool IsPacketThisType(const uint8_t * rtp_payload) {return true;};
// };

class STAP_A : public NALUTypeBase
{
	public:
		STAP_A() { Name.assign("STAP_A"); };
		virtual ~STAP_A() {};
	public:
		static const uint8_t STAP_A_ID;
};

class STAP_B : public NALUTypeBase
{
	public:
		STAP_B() { Name.assign("STAP_B"); };
		virtual ~STAP_B() {};
	public:
		static const uint8_t STAP_B_ID;
};

class MTAP_16 : public NALUTypeBase
{
	public:
		MTAP_16() { Name.assign("MTAP_16"); };
		virtual ~MTAP_16() {};
	public:
		static const uint8_t MTAP_16_ID;
};

class MTAP_24 : public NALUTypeBase
{
	public:
		MTAP_24() { Name.assign("MTAP_24"); };
		virtual ~MTAP_24() {};
	public:
		static const uint8_t MTAP_24_ID;
};

#define FU_A_ERR 	0xFF

class FU_A : public NALUTypeBase
{
	public:
		FU_A() { Name.assign("FU_A"); };
		virtual ~FU_A() {};

	public:
		/* Function: "ParseNALUHeader_*":
		 * 	Return 'FU_A_ERR'(0xFF) if error occurred */
		uint8_t ParseNALUHeader_F(const uint8_t * rtp_payload);
		uint8_t ParseNALUHeader_NRI(const uint8_t * rtp_payload);
		uint8_t ParseNALUHeader_Type(const uint8_t * rtp_payload);
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
	public:
		static const uint8_t FU_A_ID;

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

class FU_B : public NALUTypeBase
{
	public:
		FU_B() { Name.assign("FU_B"); };
		virtual ~FU_B() {};
	public:
		static const uint8_t FU_B_ID;
};

#endif
