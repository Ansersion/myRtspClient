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

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <regex.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <iostream>
#include <gtest/gtest.h>

#include "nalu_types_h264.h"

using namespace std;

TEST(NALUPayloadType, STAP_A_RegularInput_1)
{

	STAP_A STAPAType;

	uint8_t buf1[1024];
	size_t size_1;
	memset(buf1, 0, 1024);
	int fd1 = open("testMedia_nalu_type_STAP_A_1.stap", O_RDONLY);
	if((size_1 = read(fd1, buf1, 1024)) <= 0) printf("read testMedia_nalu_type_STAP_A_1.stap error\n");

	EXPECT_EQ(STAPAType.ParseNALUHeader_F(buf1), 0x00);
	EXPECT_EQ(STAPAType.ParseNALUHeader_NRI(buf1), 0x60);
	EXPECT_EQ(STAPAType.ParseNALUHeader_Type(buf1), STAP_A::STAP_A_ID);

	EXPECT_EQ(STAPAType.IsPacketThisType(buf1), true);
	EXPECT_EQ(STAPAType.IsPacketEnd(buf1), true);
}

TEST(NALUPayloadType, STAP_A_RegularInput_2)
{
	STAP_A STAPAType;

	uint8_t buf[1024];
	uint8_t writeBuf[1024];
	int size_1, size_2;
	int fdWrite;
	int fdTest;
	int fdOriginH264;
	size_t sizeTmp;

	memset(buf, 0, 1024);
	memset(writeBuf, 0, 1024);

	// copy STAP packet to h264
	fdTest = open("testMedia_nalu_type_STAP_A_1.stap", O_RDONLY);
	if((size_1 = read(fdTest, buf, 1024)) <= 0) printf("read testMedia_nalu_type_STAP_A_1.stap error\n");
	sizeTmp = STAPAType.CopyData(writeBuf, buf, size_1);
	fdWrite = open("writeSTAP_Packet1.h264", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
	if(write(fdWrite, writeBuf, sizeTmp) < 0) perror("write"); 
	close(fdWrite);
	close(fdTest);

	// Compare the h264 file from STAP packet and h264 original file
	fdOriginH264 = open("testMedia_nalu_type_STAP_A_1.h264", O_RDONLY);
	fdWrite = open("writeSTAP_Packet1.h264", O_RDONLY);

	if((size_1 = read(fdOriginH264, buf, 1024)) <= 0) printf("read testMedia_nalu_type_STAP_A_1.h264 error\n");
	if((size_2 = read(fdWrite, writeBuf, 1024)) <= 0) printf("read writeSTAP_Packet1.h264 error\n");

	if(size_1 != size_2) EXPECT_TRUE(false);
	for(int i = 0; i < size_1; i++) {
		if(writeBuf[i] != buf[i]) EXPECT_TRUE(false);
	}

	// copy STAP packet to h264
	fdTest = open("testMedia_nalu_type_STAP_A_2.stap", O_RDONLY);
	if((size_1 = read(fdTest, buf, 1024)) <= 0) printf("read testMedia_nalu_type_STAP_A_2.stap error\n");
	sizeTmp = STAPAType.CopyData(writeBuf, buf, size_1);
	fdWrite = open("writeSTAP_Packet2.h264", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
	if(write(fdWrite, writeBuf, sizeTmp) < 0) perror("write"); 
	close(fdWrite);
	close(fdTest);

	// Compare the h264 file from STAP packet and h264 original file
	fdOriginH264 = open("testMedia_nalu_type_STAP_A_2.h264", O_RDONLY);
	fdWrite = open("writeSTAP_Packet2.h264", O_RDONLY);

	if((size_1 = read(fdOriginH264, buf, 1024)) <= 0) printf("read testMedia_nalu_type_STAP_A_2.h264 error\n");
	if((size_2 = read(fdWrite, writeBuf, 1024)) <= 0) printf("read writeSTAP_Packet2.h264 error\n");

	if(size_1 != size_2) EXPECT_TRUE(false);
	for(int i = 0; i < size_1; i++) {
		if(writeBuf[i] != buf[i]) EXPECT_TRUE(false);
	}
}

TEST(NALUPayloadType, FU_A_InvalidInput)
{

	FU_A FUAType;
	uint8_t InvalidInput1[] = {0x01, 0x9e, 0xea};
	uint8_t InvalidInput2[] = {0x41, 0x9a, 0x5d};

	EXPECT_EQ(FUAType.ParseNALUHeader_F(InvalidInput1), FU_A_ERR);
	EXPECT_EQ(FUAType.ParseNALUHeader_NRI(InvalidInput1), FU_A_ERR);
	EXPECT_EQ(FUAType.ParseNALUHeader_Type(InvalidInput1), FU_A_ERR);
	EXPECT_EQ(FUAType.IsPacketThisType(InvalidInput1), false);
	EXPECT_EQ(FUAType.IsPacketStart(InvalidInput1), false);
	EXPECT_EQ(FUAType.IsPacketEnd(InvalidInput1), false);
	EXPECT_EQ(FUAType.IsPacketReserved(InvalidInput1), false);

	EXPECT_EQ(FUAType.ParseNALUHeader_F(InvalidInput2), FU_A_ERR);
	EXPECT_EQ(FUAType.ParseNALUHeader_NRI(InvalidInput2), FU_A_ERR);
	EXPECT_EQ(FUAType.ParseNALUHeader_Type(InvalidInput2), FU_A_ERR);
	EXPECT_EQ(FUAType.IsPacketThisType(InvalidInput2), false);
	EXPECT_EQ(FUAType.IsPacketStart(InvalidInput2), false);
	EXPECT_EQ(FUAType.IsPacketEnd(InvalidInput2), false);
	EXPECT_EQ(FUAType.IsPacketReserved(InvalidInput2), false);
}

TEST(NALUPayloadType, FU_A_RegularInput)
{

	FU_A FUAType;
	uint8_t RegularInput_WithStart[] = {0x5c, 0x81, 0x9a};
	uint8_t RegularInput_WithEnd[] = {0x5c, 0x41, 0x50};
	uint8_t RegularInput[] = {0x5c, 0x01, 0xf6};

	EXPECT_EQ(FUAType.ParseNALUHeader_F(RegularInput_WithStart), 0x00);
	EXPECT_EQ(FUAType.ParseNALUHeader_NRI(RegularInput_WithStart), 0x40);
	EXPECT_EQ(FUAType.ParseNALUHeader_Type(RegularInput_WithStart), 0x01);
	EXPECT_EQ(FUAType.IsPacketThisType(RegularInput_WithStart), true);
	EXPECT_EQ(FUAType.IsPacketStart(RegularInput_WithStart), true);
	EXPECT_EQ(FUAType.IsPacketEnd(RegularInput_WithStart), false);
	EXPECT_EQ(FUAType.IsPacketReserved(RegularInput_WithStart), false);

	EXPECT_EQ(FUAType.ParseNALUHeader_F(RegularInput_WithEnd), 0x00);
	EXPECT_EQ(FUAType.ParseNALUHeader_NRI(RegularInput_WithEnd), 0x40);
	EXPECT_EQ(FUAType.ParseNALUHeader_Type(RegularInput_WithEnd), 0x01);
	EXPECT_EQ(FUAType.IsPacketThisType(RegularInput_WithEnd), true);
	EXPECT_EQ(FUAType.IsPacketStart(RegularInput_WithEnd), false);
	EXPECT_EQ(FUAType.IsPacketEnd(RegularInput_WithEnd), true);
	EXPECT_EQ(FUAType.IsPacketReserved(RegularInput_WithEnd), false);

	EXPECT_EQ(FUAType.ParseNALUHeader_F(RegularInput), 0x00);
	EXPECT_EQ(FUAType.ParseNALUHeader_NRI(RegularInput), 0x40);
	EXPECT_EQ(FUAType.ParseNALUHeader_Type(RegularInput), 0x01);
	EXPECT_EQ(FUAType.IsPacketThisType(RegularInput), true);
	EXPECT_EQ(FUAType.IsPacketStart(RegularInput), false);
	EXPECT_EQ(FUAType.IsPacketEnd(RegularInput), false);
	EXPECT_EQ(FUAType.IsPacketReserved(RegularInput), false);
}

