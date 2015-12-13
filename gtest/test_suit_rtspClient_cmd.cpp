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

TEST(rtspClient, DoOPTIONS_InvalidInput)
{
	string RtspUri1("rtsp://192.15.100/ansersion");
	string RtspUri2("rtsp://192.168.15.1007/ansersion");
	string RtspUri3("ansersion:/192.168.15.100:554/ansersion");
    RtspClient Client1;
    RtspClient Client2(RtspUri2);
    RtspClient Client3(RtspUri3);

	EXPECT_NE(Client1.DoOPTIONS(RtspUri1), RTSP_NO_ERROR);
	EXPECT_NE(Client1.DoOPTIONS(), RTSP_NO_ERROR);
	EXPECT_NE(Client2.DoOPTIONS(), RTSP_NO_ERROR);
	EXPECT_NE(Client3.DoOPTIONS(), RTSP_NO_ERROR);
}

TEST(rtspClient, DoOPTIONS_RegularInput)
{
	string RtspUri1("rtsp://127.0.0.1/ansersion");
	string RtspUri2("rtsp://127.0.0.1/ansersion");
	string RtspUri3("rtsp://127.0.0.1:554/ansersion");
    RtspClient Client1;
    RtspClient Client2(RtspUri2);
    RtspClient Client3(RtspUri3);

	EXPECT_EQ(Client1.DoOPTIONS(RtspUri1), RTSP_NO_ERROR);
	EXPECT_EQ(Client1.DoOPTIONS(), RTSP_NO_ERROR);
	EXPECT_EQ(Client2.DoOPTIONS(), RTSP_NO_ERROR);
	EXPECT_EQ(Client3.DoOPTIONS(), RTSP_NO_ERROR);
}

TEST(rtspClient, DoDESCRIBE_InvalidInput)
{
	string RtspUri1("rtsp://192.15.100/ansersion");
	string RtspUri2("rtsp://192.168.15.1007/ansersion");
	string RtspUri3("ansersion:/192.168.15.100:554/ansersion");
    RtspClient Client1;
    RtspClient Client2(RtspUri2);
    RtspClient Client3(RtspUri3);

	EXPECT_NE(Client1.DoDESCRIBE(RtspUri1), RTSP_NO_ERROR);
	EXPECT_NE(Client1.DoDESCRIBE(), RTSP_NO_ERROR);
	EXPECT_NE(Client2.DoDESCRIBE(), RTSP_NO_ERROR);
	EXPECT_NE(Client3.DoDESCRIBE(), RTSP_NO_ERROR);
}

TEST(rtspClient, DoDESCRIBE_RegularInput)
{
	string RtspUri1("rtsp://127.0.0.1/ansersion");
	string RtspUri2("rtsp://127.0.0.1/ansersion");
	string RtspUri3("rtsp://127.0.0.1:554/ansersion");
    RtspClient Client1;
    RtspClient Client2(RtspUri2);
    RtspClient Client3(RtspUri3);

	EXPECT_EQ(Client1.DoDESCRIBE(RtspUri1), RTSP_NO_ERROR);
	EXPECT_EQ(Client1.DoDESCRIBE(), RTSP_NO_ERROR);
	EXPECT_EQ(Client2.DoDESCRIBE(), RTSP_NO_ERROR);
	EXPECT_EQ(Client3.DoDESCRIBE(), RTSP_NO_ERROR);
}

TEST(rtspClient, DoSETUP_InvalidInput)
{
	string RtspUri("rtsp://127.0.0.1/ansersion");
    RtspClient Client(RtspUri);

	EXPECT_NE(Client.DoSETUP(NULL), RTSP_NO_ERROR);
}

TEST(rtspClient, DoSETUP_RegularInput)
{
	string RtspUri("rtsp://127.0.0.1/ansersion");
    RtspClient Client(RtspUri);

	EXPECT_EQ(Client.DoOPTIONS(), RTSP_NO_ERROR); 
	EXPECT_TRUE(Client.IsResponse_200_OK());

	EXPECT_EQ(Client.DoDESCRIBE(), RTSP_NO_ERROR); 
	EXPECT_TRUE(Client.IsResponse_200_OK());

	EXPECT_EQ(Client.ParseSDP(), RTSP_NO_ERROR);

	EXPECT_EQ(Client.DoSETUP(), RTSP_NO_ERROR);
	EXPECT_TRUE(Client.IsResponse_200_OK());
}

TEST(rtspClient, DoPLAY_InvalidInput)
{
}

TEST(rtspClient, DoPLAY_RegularInput)
{
	string RtspUri("rtsp://127.0.0.1/ansersion");
    RtspClient Client(RtspUri);

	EXPECT_EQ(Client.DoOPTIONS(), RTSP_NO_ERROR);
	EXPECT_TRUE(Client.IsResponse_200_OK());

	EXPECT_EQ(Client.DoDESCRIBE(), RTSP_NO_ERROR);
	EXPECT_TRUE(Client.IsResponse_200_OK());

	EXPECT_EQ(Client.ParseSDP(), RTSP_NO_ERROR);

	EXPECT_EQ(Client.DoSETUP(), RTSP_NO_ERROR);
	EXPECT_TRUE(Client.IsResponse_200_OK());

	printf("start PLAY\n");
	EXPECT_EQ(Client.DoPLAY("video"), RTSP_NO_ERROR);
	EXPECT_TRUE(Client.IsResponse_200_OK());

	sleep(3);
	printf("start TEARDOWN\n");
	EXPECT_EQ(Client.DoTEARDOWN("video"), RTSP_NO_ERROR);
	EXPECT_TRUE(Client.IsResponse_200_OK());
	sleep(3);
}
