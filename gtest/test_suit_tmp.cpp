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
using namespace std;

// TEST(rtspClient, DoPLAY_RegularInput)
// {
// 	string RtspUri("rtsp://127.0.0.1/ansersion");
//     RtspClient Client(RtspUri);
// 
// 	EXPECT_EQ(Client.DoOPTIONS(), RTSP_NO_ERROR);
// 	EXPECT_TRUE(Client.IsResponse_200_OK());
// 
// 	EXPECT_EQ(Client.DoDESCRIBE(), RTSP_NO_ERROR);
// 	EXPECT_TRUE(Client.IsResponse_200_OK());
// 
// 	EXPECT_EQ(Client.ParseSDP(), RTSP_NO_ERROR);
// 
// 	EXPECT_EQ(Client.DoSETUP(), RTSP_NO_ERROR);
// 	EXPECT_TRUE(Client.IsResponse_200_OK());
// 
// 	printf("start PLAY\n");
// 	EXPECT_EQ(Client.DoPLAY("video"), RTSP_NO_ERROR);
// 	EXPECT_TRUE(Client.IsResponse_200_OK());
// 
// 	sleep(3);
// 	printf("start TEARDOWN\n");
// 	EXPECT_EQ(Client.DoTEARDOWN("video"), RTSP_NO_ERROR);
// }
