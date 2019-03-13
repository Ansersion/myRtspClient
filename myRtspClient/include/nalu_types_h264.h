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

#ifndef NALU_TYPES_H264_H
#define NALU_TYPES_H264_H

#include <string>
#include <stdint.h>
#include "nalu_types.h"

// #define NAL_UNIT_TYPE_NUM 		32
// #define PACKETIZATION_MODE_NUM 	3

#define PACKET_MODE_SINGAL_NAL 			0
#define PACKET_MODE_NON_INTERLEAVED 	1
#define PACKET_MODE_INTERLEAVED 		2

#define IS_PACKET_MODE_VALID(P) 	\
	((P) >= PACKET_MODE_SINGAL_NAL && (P) <= PACKET_MODE_INTERLEAVED)

/* More info refer to H264 'nal_unit_type' */
#define IS_NALU_TYPE_VALID_H264(N) 		\
	( \
      ((N) >= 1 && (N) <= 12) || \
      ((N) == H264TypeInterfaceSTAP_A::STAP_A_ID) || \
      ((N) == H264TypeInterfaceSTAP_B::STAP_B_ID) || \
      ((N) == H264TypeInterfaceMTAP_16::MTAP_16_ID) || \
      ((N) == H264TypeInterfaceMTAP_24::MTAP_24_ID) || \
      ((N) == H264TypeInterfaceFU_A::FU_A_ID) || \
      ((N) == H264TypeInterfaceFU_B::FU_B_ID) \
	)


/* H264TypeInterface */
class H264TypeInterface
{
    public:
		static H264TypeInterface * NalUnitType_H264[PACKETIZATION_MODE_NUM_H264][NAL_UNIT_TYPE_NUM_H264];
        virtual ~H264TypeInterface() {};
        virtual uint16_t ParseNALUHeader_F(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            uint16_t NALUHeader_F_Mask = 0x0080; // binary: 1000_0000
            return (rtp_payload[0] & NALUHeader_F_Mask);
        }
        virtual uint16_t ParseNALUHeader_NRI(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            uint16_t NALUHeader_NRI_Mask = 0x0060; // binary: 0110_0000
            return (rtp_payload[0] & NALUHeader_NRI_Mask);
        }

        virtual uint16_t ParseNALUHeader_Type(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            uint16_t NALUHeader_Type_Mask = 0x001F; // binary: 0001_1111
            return (rtp_payload[0] & NALUHeader_Type_Mask);
        }
        virtual bool IsPacketStart(const uint8_t * rtp_payload) {return true;}
        virtual bool IsPacketEnd(const uint8_t * rtp_payload) {return true;}
        virtual bool IsPacketReserved(const uint8_t * rtp_payload) {return false;}
        virtual bool IsPacketThisType(const uint8_t * rtp_payload) {return true;}
        virtual int SkipHeaderSize(const uint8_t * rtp_payload) {return 1;}
};

class H264TypeInterfaceSTAP_A : public H264TypeInterface
{
	public:
		virtual ~H264TypeInterfaceSTAP_A() {};
		virtual bool IsPacketStart(const uint8_t * rtp_payload);
		virtual bool IsPacketEnd(const uint8_t * rtp_payload);
		virtual bool IsPacketThisType(const uint8_t * rtp_payload);
	public:
		static const uint8_t STAP_A_ID;
};

class H264TypeInterfaceSTAP_B : public H264TypeInterface
{
	public:
		virtual ~H264TypeInterfaceSTAP_B() {};

	public:
		static const uint8_t STAP_B_ID;
};

class H264TypeInterfaceMTAP_16 : public H264TypeInterface
{
	public:
		virtual ~H264TypeInterfaceMTAP_16() {};
	public:
		static const uint8_t MTAP_16_ID;
};

class H264TypeInterfaceMTAP_24 : public H264TypeInterface
{
	public:
		virtual ~H264TypeInterfaceMTAP_24() {};
	public:
		static const uint8_t MTAP_24_ID;
};

#define FU_A_ERR 	0xFF

class H264TypeInterfaceFU_A : public H264TypeInterface
{
	public:
		virtual ~H264TypeInterfaceFU_A() {};

	public:
		/* Function: "ParseNALUHeader_*":
		 * 	Return 'FU_A_ERR'(0xFF) if error occurred */
		virtual uint16_t ParseNALUHeader_F(const uint8_t * rtp_payload);
		virtual uint16_t ParseNALUHeader_NRI(const uint8_t * rtp_payload);
		virtual uint16_t ParseNALUHeader_Type(const uint8_t * rtp_payload);
        virtual int SkipHeaderSize(const uint8_t * rtp_payload) {return 2;}
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

class H264TypeInterfaceFU_B : public H264TypeInterface
{
	public:
		virtual ~H264TypeInterfaceFU_B() {};
	public:
		static const uint8_t FU_B_ID;
};

class NALUTypeBase_H264 : public NALUTypeBase
{
    public: 
        static const string ENCODE_TYPE;
	public:
        NALUTypeBase_H264();
		virtual ~NALUTypeBase_H264() {};
	public:
		virtual uint16_t ParseNALUHeader_F(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_NRI(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Type(const uint8_t * RTPPayload);
		virtual bool IsPacketStart(const uint8_t * rtp_payload) {return true;}
		virtual bool IsPacketEnd(const uint8_t * rtp_payload) {return true;}
		virtual bool IsPacketReserved(const uint8_t * rtp_payload) {return false;}
		virtual bool IsPacketThisType(const uint8_t * rtp_payload);
		virtual H264TypeInterface * GetNaluRtpType(int packetization, int nalu_type_id);
		virtual std::string GetName() const { return Name; }
		virtual bool GetEndFlag() { return EndFlag; }
		virtual bool GetStartFlag() { return StartFlag; }

		// H265 interface with no use
		virtual uint16_t ParseNALUHeader_Layer_ID(const uint8_t * RTPPayload) {return 0;}
		virtual uint16_t ParseNALUHeader_Temp_ID_Plus_1(const uint8_t * RTPPayload) {return 0;}

    public:
        virtual void Init();
        virtual uint8_t * PrefixParameterOnce(uint8_t * buf, size_t * size);
        virtual bool NeedPrefixParameterOnce();
        virtual int ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo);
	 	virtual int ParsePacket(const uint8_t * RTPPayload, bool * EndFlag);
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);

        virtual void SetSPS(const string &s) { SPS.assign(s);}
        virtual void SetPPS(const string &s) { PPS.assign(s);}
        virtual const string GetSPS() { return SPS;}
        virtual const string GetPPS() { return PPS;}

        void InsertXPS() { prefixParameterOnce = true; }
        void NotInsertXPSAgain() { prefixParameterOnce = false; }
    private:
        bool prefixParameterOnce;
        string SPS;
        string PPS;
        int Packetization;
    public:
        H264TypeInterface * NALUType;
};
#endif
