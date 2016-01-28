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

#include "nalu_types_h265.h"
#include <string.h>

const uint16_t APs_H265::APs_ID_H265 = 0x30; // decimal: 48;
const uint16_t FUs_H265::FUs_ID_H265 = 0x31; // decimal: 49;

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
	NalType = NalType >> 9; // nuh_layder_id(6) + temporal_id(3) = 9
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

