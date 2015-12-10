#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <iostream>
#include <gtest/gtest.h>
#include <map>

#include "rtspClient.h"

using namespace std;

TEST(rtspClient, RtspClient_ParseSessionID_InvalidInput)
{
    RtspClient Client;
	string tmp("aslhglj");

	EXPECT_EQ(true, Client.ParseSessionID(tmp) == "");
	EXPECT_EQ(true, Client.ParseSessionID() == "");
}

TEST(rtspClient, RtspClient_ParseSessionID_RegularInput)
{
	string ResponseOfSETUP("\
			RTSP/1.0 200 OK\r\n\
			Server: VLC/2.1.6\r\n\
			Date: Sun, 06 Dec 2015 11:51:38 GMT\r\n\
			Transport: RTP/AVP/UDP;unicast;client_port=13362-13363;server_port=40774-40775;ssrc=1883F81B;mode=play\r\n\
			Session: 970756dc30b3a638;timeout=60\r\n\
			Content-Length: 0\r\n\
			Cache-Control: no-cache\r\n\
			Cseq: 3");

	RtspClient Client;
	EXPECT_EQ(true, Client.ParseSessionID(ResponseOfSETUP) == "970756dc30b3a638");
}
