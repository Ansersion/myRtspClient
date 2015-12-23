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
#include <sys/types.h>
#include <regex.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <iostream>
#include <gtest/gtest.h>

#include "nalu_types.h"

using namespace std;

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
