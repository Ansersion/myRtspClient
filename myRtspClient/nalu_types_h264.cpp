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

const string NALUTypeBase_H264::ENCODE_TYPE = "H264";

H264TypeInterface H264TypeInterface_H264Obj;
H264TypeInterfaceSTAP_A 	H264TypeInterfaceSTAP_AObj;
H264TypeInterfaceSTAP_B 	H264TypeInterfaceSTAP_BObj;
H264TypeInterfaceMTAP_16 H264TypeInterfaceMTAP_16Obj;
H264TypeInterfaceMTAP_24 H264TypeInterfaceMTAP_24Obj;
H264TypeInterfaceFU_A 	H264TypeInterfaceFU_AObj;
H264TypeInterfaceFU_B 	H264TypeInterfaceFU_BObj;

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
}

H264TypeInterface * NALUTypeBase_H264::GetNaluRtpType(int packetization, int nalu_type_id)
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
    NALUType = GetNaluRtpType(PM, NT);
    if(NULL == NALUType) {
        printf("Error(H264): Unknown NALU Type(PM=%d,NT=%d)\n", PM, NT);
        return -3;
    }

    StartFlag = NALUType->IsPacketStart(packet);
    EndFlag = NALUType->IsPacketEnd(packet);
    *EndFlagTmp = EndFlag;
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

