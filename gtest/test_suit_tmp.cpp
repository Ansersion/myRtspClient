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

#include "myRegex.h"
#include "rtspClient.h"
#include "MediaSession.h"
// #include "myRtpSession.h"

// using namespace std;
// using namespace jrtplib;

// TEST(rtspClient, TMP_TEST)
// {
// 
// 	MediaSession Ms;
// 	Ms.RTPPort = 20020;
// 	Ms.TimeRate = 8000;
// 
// 	MyRTPSession Mrs;
// 	if(!Mrs.MyRTP_SetUp(&Ms)) {
// 		printf("Msr Setup Error");
// 		return;
// 	}
// 
// 	uint8_t buf[4192];
// 	size_t size = 0;
// 	int num = 100;
// 
// 	for (int i = 1 ; i <= num ; i++)
// 	{
// 	 	Mrs.GetMyRTPData(buf, &size);
// 	 	printf("size: %lu\n", size);
// 	}
// 	EXPECT_EQ(true, true);
// }
