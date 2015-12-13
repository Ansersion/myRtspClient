#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <iostream>
#include <gtest/gtest.h>

#include "rtspClient.h"

using namespace std;

TEST(rtspClient, ToolMethods_InvalidInput)
{
	string RtspUri1("rtsp://192.15.100/ansersion");
	string RtspUri2("rtsp://192.168.15.1007/ansersion");
	string RtspUri3("ansersion:/192.168.15.100:554/ansersion");
    RtspClient Client1;
    RtspClient Client2(RtspUri2);
    RtspClient Client3(RtspUri3);

	EXPECT_LT(Client1.CreateTcpSockfd(RtspUri1), 0);
	EXPECT_LT(Client2.CreateTcpSockfd(), 0);
	EXPECT_LT(Client3.CreateTcpSockfd(), 0);

	EXPECT_EQ(Client1.GetIP(), INADDR_NONE);
	EXPECT_EQ(Client2.GetIP(RtspUri2), INADDR_NONE);
	EXPECT_EQ(Client3.GetIP(RtspUri3), INADDR_NONE);

	uint16_t RtspPort = 554;
	EXPECT_EQ(Client1.GetPort(), RtspPort);
	EXPECT_EQ(Client2.GetPort(RtspUri2), RtspPort);
	EXPECT_EQ(Client3.GetPort(RtspUri3), RtspPort);

	string Response1("");
	string Response2("UNKNOWN");
	string Response3("hsldhglreiwuo");

	RtspClient RtspResponseTest;
	EXPECT_FALSE(RtspResponseTest.IsResponse_200_OK(NULL, &Response1));
	EXPECT_FALSE(RtspResponseTest.IsResponse_200_OK(NULL, &Response2));
	EXPECT_FALSE(RtspResponseTest.IsResponse_200_OK(NULL, &Response3));

}

TEST(rtspClient, ToolMethods_RegularInput)
{
	string RtspUri1("rtsp://127.0.0.1/ansersion");
	string RtspUri2("rtsp://127.0.0.1/ansersion");
	string RtspUri3("rtsp://127.0.0.1:554/ansersion");
    RtspClient Client1;
    RtspClient Client2(RtspUri2);
    RtspClient Client3(RtspUri3);

	EXPECT_GT(Client1.CreateTcpSockfd(RtspUri1), 0);
	EXPECT_GT(Client2.CreateTcpSockfd(), 0);
	EXPECT_GT(Client3.CreateTcpSockfd(), 0);

	string IP("127.0.0.1");
	EXPECT_EQ(Client1.GetIP(), inet_addr(IP.c_str()));
	EXPECT_EQ(Client2.GetIP(RtspUri2), inet_addr(IP.c_str()));
	EXPECT_EQ(Client3.GetIP(RtspUri3), inet_addr(IP.c_str()));

	uint16_t RtspPort = 554;
	EXPECT_EQ(Client1.GetPort(), RtspPort);
	EXPECT_EQ(Client2.GetPort(RtspUri2), RtspPort);
	EXPECT_EQ(Client3.GetPort(RtspUri3), RtspPort);

	string Response_200("RTSP/1.0 200 OK\r\nServer: VLC/2.1.6\r\nDate: Sun, 06 Dec 2015 11:51:38 GMT\r\n\r\n");
	string Response_400("RTSP/1.0 400 Bad Request\r\nServer: VLC/2.1.6\r\nDate: Sun, 06 Dec 2015 11:51:38 GMT\r\n\r\n");
	string Response_500("RTSP/1.0 500 Internal Server Error\r\nServer: VLC/2.1.6\r\nDate: Sun, 06 Dec 2015 11:51:38 GMT\r\n\r\n");

	RtspClient RtspResponseTest;
	ErrorType Err;
	EXPECT_TRUE(RtspResponseTest.IsResponse_200_OK(&Err, &Response_200));
	EXPECT_EQ(Err, RTSP_RESPONSE_200);
	EXPECT_FALSE(RtspResponseTest.IsResponse_200_OK(&Err, &Response_400));
	EXPECT_EQ(Err, RTSP_RESPONSE_40X);
	EXPECT_FALSE(RtspResponseTest.IsResponse_200_OK(&Err, &Response_500));
	EXPECT_EQ(Err, RTSP_RESPONSE_50X);
}

