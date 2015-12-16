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
#include "myRtpSession.h"

using namespace std;
using namespace jrtplib;

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
