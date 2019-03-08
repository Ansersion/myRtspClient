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
// refer to RFC7798

#include "nalu_types_h265.h"
#include <string.h>
// #include <stdio.h>
#include <arpa/inet.h>

APs_H265 APs_H265Obj;
FUs_H265 FUs_H265Obj;
NALUTypeBase_H265 NaluBaseType_H265Obj;

H265TypeInterfaceAPs H265TypeInterfaceAPs_H265Obj;
H265TypeInterfaceFUs H265TypeInterfaceFUs_H265Obj;
H265TypeInterface H265TypeInterface_H265Obj;

const string NALUTypeBase_H265::ENCODE_TYPE = "H265";

H265TypeInterface * H265TypeInterface::NalUnitType_H265[PACKETIZATION_MODE_NUM_H265][NAL_UNIT_TYPE_NUM_H265] =
{
	{
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj, 
		&H265TypeInterface_H265Obj,     &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,            &H265TypeInterface_H265Obj,
		&H265TypeInterface_H265Obj, 	   NULL,                             NULL,                             NULL,                
        NULL,                      NULL,                             NULL,                             NULL, 
        &H265TypeInterfaceAPs_H265Obj,               &H265TypeInterfaceFUs_H265Obj,                    NULL,                             NULL, 
        NULL,                      NULL,                             NULL,                             NULL, 
        NULL,                      NULL,                             NULL,                             NULL, 
        NULL,                      NULL,                             NULL,                             NULL
	}
};

uint16_t NALUTypeBase_H265::ParseNALUHeader_F(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return 0;
	const uint16_t NALUHeader_F_Mask = 0x8000; // binary: 1000_0000_0000_0000
	uint16_t HeaderTmp = 0;
	HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
	HeaderTmp = HeaderTmp & NALUHeader_F_Mask;
	return HeaderTmp;
}

uint16_t NALUTypeBase_H265::ParseNALUHeader_Type(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return 0;
	const uint16_t NALUHeader_Type_Mask = 0x7E00; // binary: 0111_1110_0000_0000
	uint16_t HeaderTmp = 0;
	HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
	HeaderTmp = HeaderTmp & NALUHeader_Type_Mask;
	return HeaderTmp;
}

uint16_t NALUTypeBase_H265::ParseNALUHeader_Layer_ID(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return 0;
	const uint16_t NALUHeader_Layer_ID_Mask = 0x01F8; // binary: 0000_0001_1111_1000
	uint16_t HeaderTmp = 0;
	HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
	HeaderTmp = HeaderTmp & NALUHeader_Layer_ID_Mask;
	return HeaderTmp;
}

uint16_t NALUTypeBase_H265::ParseNALUHeader_Temp_ID_Plus_1(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return 0;
	const uint16_t NALUHeader_Temp_ID_Mask = 0x0007; // binary: 0000_0000_0000_0111
	uint16_t HeaderTmp = 0;
	HeaderTmp = ((rtp_payload[0] << 8) | rtp_payload[1]);
	HeaderTmp = HeaderTmp & NALUHeader_Temp_ID_Mask;
	return HeaderTmp;
}

bool NALUTypeBase_H265::IsPacketThisType(const uint8_t * rtp_payload)
{
	// NAL type is valid in the range of [0,40]
	uint8_t NalType = ParseNALUHeader_Type(rtp_payload);
	NalType = NalType >> (NUH_LAYER_ID_BIT_NUM + NUH_TEMPORAL_ID_PLUS1_BIT_NUM);
	return ((0 <= NalType) && (NalType <= 40));
}

size_t NALUTypeBase_H265::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;
	if(!buf || !data) return 0;

	EndFlag = IsPacketEnd(NULL);

	// NALU start code: 0x00000001 
	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	CopySize += 4; 

	memcpy(buf + CopySize, data, size);
	CopySize += size;

	return CopySize;
}

NALUTypeBase * NALUTypeBase_H265::GetNaluRtpType(int packetization, int nalu_type_id)
{
	// nalu_type_id = nalu_type_id >> (NUH_LAYER_ID_BIT_NUM + NUH_TEMPORAL_ID_PLUS1_BIT_NUM);

	// if(!IS_NALU_TYPE_VALID_H265(nalu_type_id)) {
	// 	return NULL;
	// }

	// return NALUTypeBase::NalUnitType_H265[packetization][nalu_type_id];
    /* TODO */
    return NULL;
}

int NALUTypeBase_H265::ParseParaFromSDP(SDPMediaInfo & sdpMediaInfo)
{
    map<int, map<SDP_ATTR_ENUM, string> >::iterator it = sdpMediaInfo.fmtMap.begin();
    if(it->second.find(ATTR_VPS) != it->second.end()) {
        SetVPS(it->second[ATTR_VPS]);
    }
    if(it->second.find(ATTR_SPS) != it->second.end()) {
        SetSPS(it->second[ATTR_SPS]);
    }
    if(it->second.find(ATTR_PPS) != it->second.end()) {
        SetPPS(it->second[ATTR_PPS]);
    }
    return 0;
}

const uint16_t APs_H265::APs_ID_H265 = 0x30; // decimal: 48;
const uint16_t FUs_H265::FUs_ID_H265 = 0x31; // decimal: 49;


bool APs_H265::IsPacketStart(const uint8_t * rtp_payload)
{
	return 0;
}

bool APs_H265::IsPacketEnd(const uint8_t * rtp_payload)
{
	return 0;
}

bool APs_H265::IsPacketThisType(const uint8_t * rtp_payload)
{
	return 0;
}

size_t APs_H265::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	return 0;
}


uint16_t FUs_H265::ParseNALUHeader_Type(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FUs_H265_ERR;
	// if(FUs_ID_H265 != (rtp_payload[3] & FUs_ID_H265)) return FUs_H265_ERR;

	uint8_t NALUHeader_Type_Mask = 0x3F; // binary: 0011_1111
	uint16_t NALUHeader_Type = 0;
	NALUHeader_Type |= (rtp_payload[2] & NALUHeader_Type_Mask) << (NUH_LAYER_ID_BIT_NUM + NUH_TEMPORAL_ID_PLUS1_BIT_NUM);

	return NALUHeader_Type;
}

size_t FUs_H265::CopyData(uint8_t * buf, uint8_t * data, size_t size)
{
	size_t CopySize = 0;
	if(!buf || !data) return 0;

	StartFlag = IsPacketStart(data);
	EndFlag = IsPacketEnd(data);

	uint16_t NALUHeader = 0;
	NALUHeader = (  
			ParseNALUHeader_F(data) 		| 
			ParseNALUHeader_Type(data) 		| 
			ParseNALUHeader_Layer_ID(data) 	|
			ParseNALUHeader_Temp_ID_Plus_1(data)
			);
	NALUHeader = htons(NALUHeader);

	if(StartFlag) {

		// NALU start code size
		buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
		CopySize += 4; 
		memcpy(buf + CopySize, &NALUHeader, sizeof(NALUHeader));
		CopySize += sizeof(NALUHeader);
	}
	const int FU_A_HeaderSize = 3; // NALUHeader(2) + FUHeader(1) = 3
	memcpy(buf + CopySize, data + FU_A_HeaderSize, size - FU_A_HeaderSize);
	CopySize += size - FU_A_HeaderSize;

	return CopySize;
}

bool FUs_H265::IsPacketThisType(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return false;

	uint16_t NalType = NALUTypeBase_H265::ParseNALUHeader_Type(rtp_payload);
	NalType = NalType >> (NUH_LAYER_ID_BIT_NUM + NUH_TEMPORAL_ID_PLUS1_BIT_NUM);
	return (NalType == FUs_ID_H265);
}

bool FUs_H265::IsPacketStart(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;
	uint8_t PacketS_Mask = 0x80; // binary:1000_0000
	return (rtp_payload[2] & PacketS_Mask);
}

bool FUs_H265::IsPacketEnd(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;
	uint8_t PacketE_Mask = 0x40; // binary:0100_0000
	return (rtp_payload[2] & PacketE_Mask);
}


/* */

bool H265TypeInterfaceAPs::IsPacketStart(const uint8_t * rtp_payload)
{
	return 0;
}

bool H265TypeInterfaceAPs::IsPacketEnd(const uint8_t * rtp_payload)
{
	return 0;
}

bool H265TypeInterfaceAPs::IsPacketThisType(const uint8_t * rtp_payload)
{
	return 0;
}

uint16_t H265TypeInterfaceFUs::ParseNALUHeader_Type(const uint8_t * rtp_payload)
{
	if(!rtp_payload) return FUs_H265_ERR;
	// if(FUs_ID_H265 != (rtp_payload[3] & FUs_ID_H265)) return H265TypeInterfaceFUs_ERR;

	uint8_t NALUHeader_Type_Mask = 0x3F; // binary: 0011_1111
	uint16_t NALUHeader_Type = 0;
	NALUHeader_Type |= (rtp_payload[2] & NALUHeader_Type_Mask) << (NUH_LAYER_ID_BIT_NUM + NUH_TEMPORAL_ID_PLUS1_BIT_NUM);

	return NALUHeader_Type;
}

bool H265TypeInterfaceFUs::IsPacketThisType(const uint8_t * rtp_payload)
{
	// if(!rtp_payload) return false;

	// uint16_t NalType = NALUTypeBase_H265::ParseNALUHeader_Type(rtp_payload);
	// NalType = NalType >> (NUH_LAYER_ID_BIT_NUM + NUH_TEMPORAL_ID_PLUS1_BIT_NUM);
	// return (NalType == FUs_ID_H265);
    /* TODO */
    return true;
}

bool H265TypeInterfaceFUs::IsPacketStart(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;
	uint8_t PacketS_Mask = 0x80; // binary:1000_0000
	return (rtp_payload[2] & PacketS_Mask);
}

bool H265TypeInterfaceFUs::IsPacketEnd(const uint8_t * rtp_payload)
{
	if(!IsPacketThisType(rtp_payload)) return false;
	uint8_t PacketE_Mask = 0x40; // binary:0100_0000
	return (rtp_payload[2] & PacketE_Mask);
}
