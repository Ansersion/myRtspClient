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

#include "nalu_types_h264.h"
#include "nalu_types_h265.h"
#include <string.h>
#include <sstream>
#include <iostream>
// #include <stdio.h>

using std::stringstream;
using std::cerr;
using std::endl;

STAP_A 	STAP_AObj;
STAP_B 	STAP_BObj;
MTAP_16 MTAP_16Obj;
MTAP_24 MTAP_24Obj;
FU_A 	FU_AObj;
FU_B 	FU_BObj;
NALUTypeBase_H264 NaluBaseType_H264Obj;

const string NALUTypeBase_H264::ENCODE_TYPE = "H264";

H264TypeInterface H264TypeInterface_H264Obj;
H264TypeInterfaceSTAP_A 	H264TypeInterfaceSTAP_AObj;
H264TypeInterfaceSTAP_B 	H264TypeInterfaceSTAP_BObj;
H264TypeInterfaceMTAP_16 H264TypeInterfaceMTAP_16Obj;
H264TypeInterfaceMTAP_24 H264TypeInterfaceMTAP_24Obj;
H264TypeInterfaceFU_A 	H264TypeInterfaceFU_AObj;
H264TypeInterfaceFU_B 	H264TypeInterfaceFU_BObj;


NALUTypeBase * NALUTypeBase::NalUnitType_H264[PACKETIZATION_MODE_NUM_H264][NAL_UNIT_TYPE_NUM_H264] =
{
	/* Packetization Mode: Single NAL */ 
	{
		NULL,                      &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj, 
		&NaluBaseType_H264Obj,     &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj, 
		&NaluBaseType_H264Obj,     &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj, 
		&NaluBaseType_H264Obj,     NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL
	},

	/* Packetization Mode: Non-interleaved */ 
	{
		NULL,                      &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj, 
		&NaluBaseType_H264Obj,     &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj, 
		&NaluBaseType_H264Obj,     &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj,            &NaluBaseType_H264Obj, 
		&NaluBaseType_H264Obj,     NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		&STAP_AObj,                NULL,                             NULL,                             NULL, 
		&FU_AObj,                  NULL,                             NULL,                             NULL
	},

	/* Packetization Mode: Interleaved */ 
	{
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      &STAP_BObj,                       &MTAP_16Obj,                      &MTAP_24Obj, 
		&FU_AObj,                  &FU_BObj,                         NULL,                             NULL
	}
};

H264TypeInterface * H264TypeInterface::NalUnitType_H264[PACKETIZATION_MODE_NUM_H264][NAL_UNIT_TYPE_NUM_H264] =
{
	/* Packetization Mode: Single NAL */ 
	{
		NULL,                      &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj, 
		&H264TypeInterface_H264Obj,     &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj, 
		&H264TypeInterface_H264Obj,     &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj, 
		&H264TypeInterface_H264Obj,     NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL
	},

	/* Packetization Mode: Non-interleaved */ 
	{
		NULL,                      &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj, 
		&H264TypeInterface_H264Obj,     &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj, 
		&H264TypeInterface_H264Obj,     &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj,            &H264TypeInterface_H264Obj, 
		&H264TypeInterface_H264Obj,     NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		&H264TypeInterfaceSTAP_AObj,                NULL,                             NULL,                             NULL, 
		&H264TypeInterfaceFU_AObj,                  NULL,                             NULL,                             NULL
	},

	/* Packetization Mode: Interleaved */ 
	{
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      NULL,                             NULL,                             NULL, 
		NULL,                      &H264TypeInterfaceSTAP_BObj,                       &H264TypeInterfaceMTAP_16Obj,                      &H264TypeInterfaceMTAP_24Obj, 
		&H264TypeInterfaceFU_AObj,                  &H264TypeInterfaceFU_BObj,                         NULL,                             NULL
	}
};

NALUTypeBase_H264::NALUTypeBase_H264():NALUTypeBase()
{
    prefixParameterOnce = true;
    Packetization = PACKET_MODE_SINGAL_NAL;
    NALUType = NULL;
    SPS.assign("");
    PPS.assign("");
}


void NALUTypeBase_H264::Init() 
{
    InsertXPS();
}

uint16_t NALUTypeBase_H264::ParseNALUHeader_F(const uint8_t * rtp_payload) 
{
	if(!rtp_payload) return 0;
	uint16_t NALUHeader_F_Mask = 0x0080; // binary: 1000_0000
	return (rtp_payload[0] & NALUHeader_F_Mask);
}

uint16_t NALUTypeBase_H264::ParseNALUHeader_NRI(const uint8_t * rtp_payload) 
{
	if(!rtp_payload) return 0;
	uint16_t NALUHeader_NRI_Mask = 0x0060; // binary: 0110_0000
	return (rtp_payload[0] & NALUHeader_NRI_Mask);
}

uint16_t NALUTypeBase_H264::ParseNALUHeader_Type(const uint8_t * rtp_payload) 
{
	if(!rtp_payload) return 0;
	uint16_t NALUHeader_Type_Mask = 0x001F; // binary: 0001_1111
	return (rtp_payload[0] & NALUHeader_Type_Mask);
}

bool NALUTypeBase_H264::IsPacketThisType(const uint8_t * rtp_payload) 
{
	// NAL type is valid in the range of [1,12]
	uint16_t NalType = ParseNALUHeader_Type(rtp_payload);
	return ((1 <= NalType) && (NalType <= 12));
}

size_t NALUTypeBase_H264::CopyData(uint8_t * buf, uint8_t * data, size_t size) 
{
	size_t CopySize = 0;
	if(!buf || !data) return 0;

    if(!NALUType) return 0;

	uint8_t NALUHeader = 0;
	NALUHeader = (uint8_t)(  
			NALUType->ParseNALUHeader_F(data)      | 
			NALUType->ParseNALUHeader_NRI(data)    | 
			NALUType->ParseNALUHeader_Type(data)
			);

	if(StartFlag) {

		// NALU start code size
		buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
		CopySize += 4; 
		memcpy(buf + CopySize, &NALUHeader, sizeof(NALUHeader));
		CopySize += sizeof(NALUHeader);
	}
	const int FU_A_HeaderSize = 2;
	memcpy(buf + CopySize, data + FU_A_HeaderSize, size - FU_A_HeaderSize);
	CopySize += size - FU_A_HeaderSize;

	return CopySize;
    /*
	size_t CopySize = 0;
	if(!buf || !data) return 0;

	EndFlag = IsPacketEnd(NULL);

	// NALU start code: 0x00000001 
	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	CopySize += 4; 

	memcpy(buf + CopySize, data, size);
	CopySize += size;

	return CopySize;
    */
}

NALUTypeBase * NALUTypeBase_H264::GetNaluRtpType(int packetization, int nalu_type_id)
{
	if(!IS_NALU_TYPE_VALID_H264(nalu_type_id)) {
		return NULL;
	}

	return NALUTypeBase::NalUnitType_H264[packetization][nalu_type_id];
}

H264TypeInterface * NALUTypeBase_H264::GetNaluRtpType2(int packetization, int nalu_type_id)
{
	if(!IS_NALU_TYPE_VALID_H264(nalu_type_id)) {
		return NULL;
	}

	return H264TypeInterface::NalUnitType_H264[packetization][nalu_type_id];
}

uint8_t * NALUTypeBase_H264::PrefixParameterOnce(uint8_t * buf, size_t * size)
{
    if(!buf) return NULL;
    if(!size) return NULL;
    const size_t NALU_StartCodeSize = 4;
    size_t SizeTmp = 0;

    if(!NALUTypeBase::PrefixXPS(buf + (*size), &SizeTmp, SPS) || SizeTmp <= NALU_StartCodeSize) {
        fprintf(stderr, "\033[31mWARNING: No [X]PS\033[0m\n");
        return NULL;
    }
    *size += SizeTmp;

    if(!NALUTypeBase::PrefixXPS(buf + (*size), &SizeTmp, PPS) || SizeTmp <= NALU_StartCodeSize) {
        fprintf(stderr, "\033[31mWARNING: No [X]PS\033[0m\n");
        return NULL;
    }
    *size += SizeTmp;

    NotInsertXPSAgain();

    return buf;
}

bool NALUTypeBase_H264::NeedPrefixParameterOnce() 
{
    static int x = 0;
    if(x++ > 30) {
       // InsertXPS(); 
       x = 0;
    }
    return prefixParameterOnce;
}

int NALUTypeBase_H264::ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo)
{
    map<int, map<SDP_ATTR_ENUM, string> >::iterator it = sdpMediaInfo.fmtMap.begin();
    if(it->second.find(ATTR_SPS) != it->second.end()) {
        SetSPS(it->second[ATTR_SPS]);
    }
    if(it->second.find(ATTR_PPS) != it->second.end()) {
        SetPPS(it->second[ATTR_PPS]);
    }
    if(it->second.find(PACK_MODE) != it->second.end()) {
        stringstream ssPackMode;
        ssPackMode << it->second[PACK_MODE];
        ssPackMode >> Packetization;
        // cout << "debug: Packetization=" << NewMediaSession.Packetization << endl;;
    }
    return 0;
}

int NALUTypeBase_H264::ParsePacket(const uint8_t * packet, bool * EndFlagTmp)
{
    if(!packet || !EndFlagTmp) {
        return -1;
    }
	if(!IS_PACKET_MODE_VALID(Packetization)) {
		cerr << "Error(H264): Invalid Packetization Mode" << endl;
		return -2;
	}
		 
    int PM = Packetization;
    int NT = ParseNALUHeader_Type(packet);
    NALUType = GetNaluRtpType2(PM, NT);
    if(NULL == NALUType) {
        printf("Error(H264): Unknown NALU Type(PM=%d,NT=%d)\n", PM, NT);
        return -3;
    }

    StartFlag = NALUType->IsPacketStart(packet);
    EndFlag = NALUType->IsPacketEnd(packet);
    *EndFlagTmp = EndFlag;
    return 0;
}

std::string STAP_A::GetName() const 
{
	return NALUTypeBase_H264::GetName();
}

bool STAP_A::GetEndFlag()
{
	return NALUTypeBase_H264::GetEndFlag();
}

bool STAP_A::GetStartFlag()
{
	return NALUTypeBase_H264::GetStartFlag();
}

std::string STAP_B::GetName() const 
{
	return NALUTypeBase_H264::GetName();
}

bool STAP_B::GetEndFlag()
{
	return NALUTypeBase_H264::GetEndFlag();
}

bool STAP_B::GetStartFlag()
{
	return NALUTypeBase_H264::GetStartFlag();
}


uint16_t STAP_B::ParseNALUHeader_F(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t STAP_B::ParseNALUHeader_NRI(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t STAP_B::ParseNALUHeader_Type(const uint8_t * RTPPayload)
{
    return 0;
}

bool STAP_B::IsPacketStart(const uint8_t * rtp_payload)
{
    return false;
}

bool STAP_B::IsPacketEnd(const uint8_t * rtp_payload)
{
    return 0;
}

bool STAP_B::IsPacketThisType(const uint8_t * rtp_payload)
{
    return 0;
}

size_t STAP_B::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
    return 0;
}


uint16_t MTAP_16::ParseNALUHeader_F(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t MTAP_16::ParseNALUHeader_NRI(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t MTAP_16::ParseNALUHeader_Type(const uint8_t * RTPPayload)
{
    return 0;
}

bool MTAP_16::IsPacketStart(const uint8_t * rtp_payload)
{
    return false;
}

bool MTAP_16::IsPacketEnd(const uint8_t * rtp_payload)
{
    return 0;
}

bool MTAP_16::IsPacketThisType(const uint8_t * rtp_payload)
{
    return 0;
}

size_t MTAP_16::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
    return 0;
}


uint16_t MTAP_24::ParseNALUHeader_F(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t MTAP_24::ParseNALUHeader_NRI(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t MTAP_24::ParseNALUHeader_Type(const uint8_t * RTPPayload)
{
    return 0;
}

bool MTAP_24::IsPacketStart(const uint8_t * rtp_payload)
{
    return false;
}

bool MTAP_24::IsPacketEnd(const uint8_t * rtp_payload)
{
    return 0;
}

bool MTAP_24::IsPacketThisType(const uint8_t * rtp_payload)
{
    return 0;
}

size_t MTAP_24::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
    return 0;
}

const uint8_t STAP_A::STAP_A_ID = 0x18; // decimal: 24
const uint8_t STAP_B::STAP_B_ID = 0x19; // decimal: 25
const uint8_t MTAP_16::MTAP_16_ID = 0x1A; // decimal: 26
const uint8_t MTAP_24::MTAP_24_ID = 0x1B; // decimal: 27
const uint8_t FU_A::FU_A_ID = 0x1C; // decimal: 28
const uint8_t FU_B::FU_B_ID = 0x1D; // decimal: 29

uint16_t STAP_A::ParseNALUHeader_F(const uint8_t * RTPPayload)
{
	return NALUTypeBase_H264::ParseNALUHeader_F(RTPPayload);
}

uint16_t STAP_A::ParseNALUHeader_NRI(const uint8_t * RTPPayload)
{
	return NALUTypeBase_H264::ParseNALUHeader_NRI(RTPPayload);
}

uint16_t STAP_A::ParseNALUHeader_Type(const uint8_t * RTPPayload)
{
	return NALUTypeBase_H264::ParseNALUHeader_Type(RTPPayload);
}

bool STAP_A::IsPacketStart(const uint8_t * rtp_payload) 
{
	return false;
}

bool STAP_A::IsPacketEnd(const uint8_t * rtp_payload)
{
	return true;
}

bool STAP_A::IsPacketThisType(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return false;
	return (STAP_A_ID == (rtp_payload[0] & STAP_A_ID));
}

size_t STAP_A::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;
	uint16_t NALU_Size = 0;
	uint8_t * DataPointer = data;

	if(!buf || !data) return 0;

	StartFlag = IsPacketStart(data);
	EndFlag = IsPacketEnd(data);

	uint8_t STAP_A_Header = 0;
	const int STAP_A_HeaderSize = sizeof(STAP_A_Header);
	DataPointer += STAP_A_HeaderSize;

	do {
		// Not enough data left or only padding data left;
		if((size_t)(DataPointer - data) >= size - sizeof(NALU_Size)) break;

		// Transform network byte order to host order
		NALU_Size = (uint16_t)(*DataPointer); DataPointer++;
		NALU_Size = (NALU_Size << 8) + (uint16_t)(*DataPointer); DataPointer++;
		if(0 == NALU_Size) break;

		// NALU start code: 0x00000001 
		buf[CopySize + 0] = 0; buf[CopySize + 1] = 0; buf[CopySize + 2] = 0; buf[CopySize + 3] = 1;
		CopySize += 4; 
		memcpy(buf + CopySize, DataPointer, NALU_Size);
		CopySize += NALU_Size;
		DataPointer += NALU_Size;
	} while(0 != NALU_Size);

	return CopySize;
}

bool FU_A::IsPacketThisType(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return false;
	return (FU_A_ID == (rtp_payload[0] & FU_A_ID));
}

uint16_t FU_A::ParseNALUHeader_F(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FU_A_ERR;
	if(FU_A_ID != (rtp_payload[0] & FU_A_ID)) return FU_A_ERR;

	uint16_t NALUHeader_F_Mask = 0x0080; // binary: 1000_0000

	// "F" at the byte of rtp_payload[0]
	return (rtp_payload[0] & NALUHeader_F_Mask);
}

uint16_t FU_A::ParseNALUHeader_NRI(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FU_A_ERR;
	if(FU_A_ID != (rtp_payload[0] & FU_A_ID)) return FU_A_ERR;

	uint16_t NALUHeader_NRI_Mask = 0x0060; // binary: 0110_0000

	// "NRI" at the byte of rtp_payload[0]
	return (rtp_payload[0] & NALUHeader_NRI_Mask);

}

uint16_t FU_A::ParseNALUHeader_Type(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FU_A_ERR;
	if(FU_A_ID != (rtp_payload[0] & FU_A_ID)) return FU_A_ERR;

	uint16_t NALUHeader_Type_Mask = 0x001F; // binary: 0001_1111

	// "Type" at the byte of rtp_payload[0]
	return (rtp_payload[1] & NALUHeader_Type_Mask);
}

bool FU_A::IsPacketStart(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;

	uint8_t PacketS_Mask = 0x80; // binary:1000_0000

	return (rtp_payload[1] & PacketS_Mask);
}

bool FU_A::IsPacketEnd(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;

	uint8_t PacketE_Mask = 0x40; // binary:0100_0000

	return (rtp_payload[1] & PacketE_Mask);
}

bool FU_A::IsPacketReserved(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;

	uint8_t PacketR_Mask = 0x20; // binary:0010_0000

	return (rtp_payload[1] & PacketR_Mask);
}

size_t FU_A::CopyData(uint8_t * buf, uint8_t * data, size_t size) 
{
	size_t CopySize = 0;
	if(!buf || !data) return 0;

	StartFlag = IsPacketStart(data);
	EndFlag = IsPacketEnd(data);

	uint8_t NALUHeader = 0;
	NALUHeader = (uint8_t)(  
			ParseNALUHeader_F(data)      | 
			ParseNALUHeader_NRI(data)    | 
			ParseNALUHeader_Type(data)
			);

	if(StartFlag) {

		// NALU start code size
		buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
		CopySize += 4; 
		memcpy(buf + CopySize, &NALUHeader, sizeof(NALUHeader));
		CopySize += sizeof(NALUHeader);
	}
	const int FU_A_HeaderSize = 2;
	memcpy(buf + CopySize, data + FU_A_HeaderSize, size - FU_A_HeaderSize);
	CopySize += size - FU_A_HeaderSize;

	return CopySize;

    /*
	size_t CopySize = 0;
	if(!buf || !data) return 0;

	uint8_t NALUHeader = 0;
	NALUHeader = (uint8_t)(  
			ParseNALUHeader_F(data)      | 
			ParseNALUHeader_NRI(data)    | 
			ParseNALUHeader_Type(data)
			);

	if(StartFlag) {

		// NALU start code size
		buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
		CopySize += 4; 
		memcpy(buf + CopySize, &NALUHeader, sizeof(NALUHeader));
		CopySize += sizeof(NALUHeader);
	}
	const int FU_A_HeaderSize = 2;
	memcpy(buf + CopySize, data + FU_A_HeaderSize, size - FU_A_HeaderSize);
	CopySize += size - FU_A_HeaderSize;

	return CopySize;
    */
}


uint16_t FU_B::ParseNALUHeader_F(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t FU_B::ParseNALUHeader_NRI(const uint8_t * RTPPayload)
{
    return 0;
}

uint16_t FU_B::ParseNALUHeader_Type(const uint8_t * RTPPayload)
{
    return 0;
}

bool FU_B::IsPacketStart(const uint8_t * rtp_payload)
{
    return false;
}

bool FU_B::IsPacketEnd(const uint8_t * rtp_payload)
{
    return 0;
}

bool FU_B::IsPacketThisType(const uint8_t * rtp_payload)
{
    return 0;
}

size_t FU_B::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
    return 0;
}

const uint8_t H264TypeInterfaceSTAP_A::STAP_A_ID = 0x18; // decimal: 24
const uint8_t H264TypeInterfaceSTAP_B::STAP_B_ID = 0x19; // decimal: 25
const uint8_t H264TypeInterfaceMTAP_16::MTAP_16_ID = 0x1A; // decimal: 26
const uint8_t H264TypeInterfaceMTAP_24::MTAP_24_ID = 0x1B; // decimal: 27
const uint8_t H264TypeInterfaceFU_A::FU_A_ID = 0x1C; // decimal: 28
const uint8_t H264TypeInterfaceFU_B::FU_B_ID = 0x1D; // decimal: 29

bool H264TypeInterfaceSTAP_A::IsPacketStart(const uint8_t * rtp_payload) 
{
	return false;
}

bool H264TypeInterfaceSTAP_A::IsPacketEnd(const uint8_t * rtp_payload)
{
	return true;
}

bool H264TypeInterfaceSTAP_A::IsPacketThisType(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return false;
	return (STAP_A_ID == (rtp_payload[0] & STAP_A_ID));
}

bool H264TypeInterfaceFU_A::IsPacketThisType(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return false;
	return (FU_A_ID == (rtp_payload[0] & FU_A_ID));
}

uint16_t H264TypeInterfaceFU_A::ParseNALUHeader_F(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FU_A_ERR;
	if(FU_A_ID != (rtp_payload[0] & FU_A_ID)) return FU_A_ERR;

	uint16_t NALUHeader_F_Mask = 0x0080; // binary: 1000_0000

	// "F" at the byte of rtp_payload[0]
	return (rtp_payload[0] & NALUHeader_F_Mask);
}

uint16_t H264TypeInterfaceFU_A::ParseNALUHeader_NRI(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FU_A_ERR;
	if(FU_A_ID != (rtp_payload[0] & FU_A_ID)) return FU_A_ERR;

	uint16_t NALUHeader_NRI_Mask = 0x0060; // binary: 0110_0000

	// "NRI" at the byte of rtp_payload[0]
	return (rtp_payload[0] & NALUHeader_NRI_Mask);

}

uint16_t H264TypeInterfaceFU_A::ParseNALUHeader_Type(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FU_A_ERR;
	if(FU_A_ID != (rtp_payload[0] & FU_A_ID)) return FU_A_ERR;

	uint16_t NALUHeader_Type_Mask = 0x001F; // binary: 0001_1111

	// "Type" at the byte of rtp_payload[0]
	return (rtp_payload[1] & NALUHeader_Type_Mask);
}

bool H264TypeInterfaceFU_A::IsPacketStart(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;

	uint8_t PacketS_Mask = 0x80; // binary:1000_0000

	return (rtp_payload[1] & PacketS_Mask);
}

bool H264TypeInterfaceFU_A::IsPacketEnd(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;

	uint8_t PacketE_Mask = 0x40; // binary:0100_0000

	return (rtp_payload[1] & PacketE_Mask);
}

bool H264TypeInterfaceFU_A::IsPacketReserved(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;

	uint8_t PacketR_Mask = 0x20; // binary:0010_0000

	return (rtp_payload[1] & PacketR_Mask);
}

