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

/* refer to RFC7798 */
#ifndef NALU_TYPES_H265_H
#define NALU_TYPES_H265_H

#include <string>
#include <stdint.h>
#include "nalu_types.h"

#define NAL_UNIT_TYPE_BIT_NUM           6
#define NUH_LAYER_ID_BIT_NUM            6
#define NUH_TEMPORAL_ID_PLUS1_BIT_NUM   3

#define FUs_H265_ERR 	0xFFFF

/* More info refer to H265 'nal_unit_type' */
#define IS_NALU_TYPE_VALID_H265(N) 		\
	( \
      ((N) >= 0 && (N) <= 40) || \
      ((N) == H265TypeInterfaceAPs::APs_ID_H265) || \
      ((N) == H265TypeInterfaceFUs::FUs_ID_H265) \
	)

/* H265TypeInterface */
class H265TypeInterface
{
    public:
		static H265TypeInterface * NalUnitType_H265[PACKETIZATION_MODE_NUM_H265][NAL_UNIT_TYPE_NUM_H265];
        virtual ~H265TypeInterface() {};
        virtual uint16_t ParseNALUHeader_F(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            const uint16_t NALUHeader_F_Mask = 0x8000; // binary: 1000_0000_0000_0000
            uint16_t HeaderTmp = 0;
            HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
            HeaderTmp = HeaderTmp & NALUHeader_F_Mask;
            return HeaderTmp;
        }
        virtual uint16_t ParseNALUHeader_NRI(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            uint16_t NALUHeader_NRI_Mask = 0x0060; // binary: 0110_0000
            return (rtp_payload[0] & NALUHeader_NRI_Mask);
        }

        virtual uint16_t ParseNALUHeader_Type(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            const uint16_t NALUHeader_Type_Mask = 0x7E00; // binary: 0111_1110_0000_0000
            uint16_t HeaderTmp = 0;
            HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
            HeaderTmp = HeaderTmp & NALUHeader_Type_Mask;
            return HeaderTmp;
        }

        virtual uint16_t ParseNALUHeader_Layer_ID(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            const uint16_t NALUHeader_Layer_ID_Mask = 0x01F8; // binary: 0000_0001_1111_1000
            uint16_t HeaderTmp = 0;
            HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
            HeaderTmp = HeaderTmp & NALUHeader_Layer_ID_Mask;
            return HeaderTmp;
        }

        virtual uint16_t ParseNALUHeader_Temp_ID_Plus_1(const uint8_t * rtp_payload) {
            if(!rtp_payload) return 0;
            const uint16_t NALUHeader_Temp_ID_Mask = 0x0007; // binary: 0000_0000_0000_0111
            uint16_t HeaderTmp = 0;
            HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
            HeaderTmp = HeaderTmp & NALUHeader_Temp_ID_Mask;
            return HeaderTmp;
        }

        virtual bool IsPacketStart(const uint8_t * rtp_payload) {return true;}
        virtual bool IsPacketEnd(const uint8_t * rtp_payload) {return true;}
        virtual bool IsPacketReserved(const uint8_t * rtp_payload) {return false;}
        virtual bool IsPacketThisType(const uint8_t * rtp_payload) {return true;}
        virtual int SkipHeaderSize(const uint8_t * rtp_payload) {return 2;}
};


class NALUTypeBase_H265 : public NALUTypeBase
{
    public:
        static const string ENCODE_TYPE;
	public:
		NALUTypeBase_H265();
		virtual ~NALUTypeBase_H265() {};
	public:
		virtual uint16_t ParseNALUHeader_F(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Type(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Layer_ID(const uint8_t * RTPPayload);
		virtual uint16_t ParseNALUHeader_Temp_ID_Plus_1(const uint8_t * RTPPayload);
		virtual bool IsPacketStart(const uint8_t * rtp_payload) { return true; }
		virtual bool IsPacketEnd(const uint8_t * rtp_payload) { return true; }
		virtual bool IsPacketThisType(const uint8_t * rtp_payload);
		H265TypeInterface * GetNaluRtpType(int packetization, int nalu_type_id);
		virtual std::string GetName() const { return Name; }
		virtual bool GetEndFlag() { return EndFlag; }
		virtual bool GetStartFlag() { return StartFlag; }
    public:
        virtual void SetVPS(const string &s) { VPS.assign(s);}
        virtual void SetSPS(const string &s) { SPS.assign(s);}
        virtual void SetPPS(const string &s) { PPS.assign(s);}
        virtual const string GetVPS() { return VPS;}
        virtual const string GetSPS() { return SPS;}
        virtual const string GetPPS() { return PPS;}
    public:
        virtual void Init();
        virtual uint8_t * PrefixParameterOnce(uint8_t * buf, size_t * size);
        virtual bool NeedPrefixParameterOnce();
        virtual int ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo);
	 	virtual int ParsePacket(const uint8_t * RTPPayload, bool * EndFlag);
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
        void InsertXPS() { prefixParameterOnce = true; }
        void NotInsertXPSAgain() { prefixParameterOnce = false; }

	protected:
        bool prefixParameterOnce;
        string VPS;
        string SPS;
        string PPS;
        // int Packetization;
		// std::string Name;
		// bool EndFlag;
		// bool StartFlag;

    public:
        H265TypeInterface * NALUType;
};

class H265TypeInterfaceAPs : public H265TypeInterface
{
	public:
		virtual ~H265TypeInterfaceAPs() {};

	public:
		virtual bool IsPacketStart(const uint8_t * rtp_payload);
		virtual bool IsPacketEnd(const uint8_t * rtp_payload);
		virtual bool IsPacketThisType(const uint8_t * rtp_payload);
        virtual int SkipHeaderSize(const uint8_t * rtp_payload) {return 2;}

	public:
		static const uint16_t APs_ID_H265;
};

class H265TypeInterfaceFUs : public H265TypeInterface
{
	public:
		virtual ~H265TypeInterfaceFUs() {};
	public:
		/* Function: "ParseNALUHeader_*":
		 * 	Return 'FU_A_ERR'(0xFF) if error occurred */
		virtual uint16_t ParseNALUHeader_Type(const uint8_t * RTPPayload);
        virtual int SkipHeaderSize(const uint8_t * rtp_payload) {return 3;}
	public:
		static const uint16_t FUs_ID_H265;

	public:
		/* if FU_A payload type */
		bool IsPacketThisType(const uint8_t * rtp_payload);

		/* Packet Start Flag */
		bool IsPacketStart(const uint8_t * rtp_payload);

		/* Packet End Flag */
		bool IsPacketEnd(const uint8_t * rtp_payload);

};
#endif
