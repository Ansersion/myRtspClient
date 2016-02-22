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
#include <map>

#include "rtspClient.h"

using namespace std;

TEST(rtspClient, RtspClient_ParseSDP_InvalidInput)
{
    RtspClient Client;

	string Invalid_SDP1("");
	string Invalid_SDP2("halsdjg3woiu");

	map<string, MediaSession> MediaSessionMap;

	Client.ParseSDP();
	MediaSessionMap = Client.GetMediaSessions();
	EXPECT_EQ(MediaSessionMap.empty(), true);

	Client.ParseSDP(Invalid_SDP1);
	MediaSessionMap = Client.GetMediaSessions();
	EXPECT_EQ(MediaSessionMap.empty(), true);

	Client.ParseSDP(Invalid_SDP2);
	MediaSessionMap = Client.GetMediaSessions();
	EXPECT_EQ(MediaSessionMap.empty(), true);
}

TEST(rtspClient, RtspClient_ParseSDP_1_RegularInput)
{
	string SDP("\
	RTSP/1.0 200 OK\r\n\
		Server: VLC/2.1.6\r\n\
		Date: Sun, 06 Dec 2015 11:51:38 GMT\r\n\
		Content-Type: application/sdp\r\n\
		Content-Base: rtsp://127.0.0.1:554/ansersion\r\n\
		Content-Length: 572\r\n\
		Cache-Control: no-cache\r\n\
		Cseq: 2\r\n\
\r\n\
		v=0\r\n\
		o=- 15712671843665285047 15712671843665285047 IN IP4 localhost.localdomain\r\n\
		s=Unnamed\r\n\
		i=N/A\r\n\
		c=IN IP4 0.0.0.0\r\n\
		t=0 0\r\n\
		a=tool:vlc 2.1.6\r\n\
		a=recvonly\r\n\
		a=type:broadcast\r\n\
		a=charset:UTF-8\r\n\
		a=control:rtsp://127.0.0.1:554/ansersion\r\n\
		m=audio 0 RTP/AVP 14\r\n\
		b=AS:128\r\n\
		b=RR:0\r\n\
		a=rtpmap:14 MPA/90000\r\n\
		a=control:rtsp://127.0.0.1:554/ansersion/trackID=0\r\n\
		m=video 0 RTP/AVP 96\r\n\
		b=RR:0\r\n\
		a=rtpmap:96 H264/90000\r\n\
		a=fmtp:96 packetization-mode=1;profile-level-id=64000c;sprop-parameter-sets=Z2QADKzZQUH7ARAAAGewAA/DqPFCmWA=,aOvjyyLA;\r\n\
		a=control:rtsp://127.0.0.1:554/ansersion/trackID=1\r\n\r\n");

	RtspClient Client;
	Client.ParseSDP(SDP);
	// multimap<string, string> SDPInfo = Client.GetSDPInfo();
	// typedef multimap<string, string>::iterator It_type;
	// pair<It_type, It_type> p = SDPInfo.equal_range("s");
	// It_type It = p.first;
	map<string, MediaSession> MediaSessionMap = Client.GetMediaSessions();

	// EXPECT_EQ(true, It->second == "Unnamed");
	// EXPECT_EQ(SDPInfo.count("a"), 10);
	// EXPECT_EQ(SDPInfo.count("m"), 2);
	// EXPECT_EQ(SDPInfo.count("v"), 1);
	// EXPECT_EQ(SDPInfo.count("x"), 0);

	EXPECT_EQ(true, MediaSessionMap["audio"].MediaType == "audio");
	EXPECT_EQ(true, MediaSessionMap["audio"].EncodeType == "MPA");
	EXPECT_EQ(true, MediaSessionMap["audio"].Protocol == "RTP/AVP");
	EXPECT_EQ(true, MediaSessionMap["audio"].TimeRate == 90000);
	EXPECT_EQ(true, MediaSessionMap["audio"].ControlURI == "rtsp://127.0.0.1:554/ansersion/trackID=0");

	EXPECT_EQ(1, MediaSessionMap["audio"].PayloadType.size());
	for(vector<int>::iterator it = MediaSessionMap["audio"].PayloadType.begin(); it != MediaSessionMap["audio"].PayloadType.end(); it++) {
		EXPECT_EQ(14, *it);
	}

	EXPECT_EQ(true, MediaSessionMap["video"].MediaType == "video");
	EXPECT_EQ(true, MediaSessionMap["video"].EncodeType == "H264");
	EXPECT_EQ(true, MediaSessionMap["video"].Protocol == "RTP/AVP");
	EXPECT_EQ(true, MediaSessionMap["video"].TimeRate == 90000);
	EXPECT_EQ(true, MediaSessionMap["video"].ControlURI == "rtsp://127.0.0.1:554/ansersion/trackID=1");
	EXPECT_EQ(true, MediaSessionMap["video"].Packetization == 1);

	EXPECT_EQ(1, MediaSessionMap["audio"].PayloadType.size());
	for(vector<int>::iterator it = MediaSessionMap["video"].PayloadType.begin(); it != MediaSessionMap["video"].PayloadType.end(); it++) {
		EXPECT_EQ(96, *it);
	}
}

TEST(rtspClient, RtspClient_ParseSDP_2_RegularInput)
{
	string SDP("\
			v=0\r\n\
			o=- 1456130741857568 1 IN IP4 192.168.1.162\r\n\
			s=Session streamed by testOnDemandRTSPServer\r\n\
			i=mp3AudioTest\r\n\
			t=0 0\r\n\
			a=tool:LIVE555 Streaming Media v2015.12.22\r\n\
			a=type:broadcast\r\n\
			a=control:*\r\n\
			a=range:npt=0-163.524\r\n\
			a=x-qt-text-nam:Session streamed by testOnDemandRTSPServer\r\n\
			a=x-qt-text-inf:mp3AudioTest\r\n\
			m=audio 0 RTP/AVP 14\r\n\
			c=IN IP4 0.0.0.0\r\n\
			b=AS:128\r\n\
			a=control:track1\r\n");

	RtspClient Client;
	Client.ParseSDP(SDP);
	// multimap<string, string> SDPInfo = Client.GetSDPInfo();
	// typedef multimap<string, string>::iterator It_type;
	// pair<It_type, It_type> p = SDPInfo.equal_range("s");
	// It_type It = p.first;
	map<string, MediaSession> MediaSessionMap = Client.GetMediaSessions();

	// EXPECT_EQ(true, It->second == "Unnamed");
	// EXPECT_EQ(SDPInfo.count("a"), 10);
	// EXPECT_EQ(SDPInfo.count("m"), 2);
	// EXPECT_EQ(SDPInfo.count("v"), 1);
	// EXPECT_EQ(SDPInfo.count("x"), 0);

	EXPECT_EQ(true, MediaSessionMap["audio"].MediaType == "audio");
	EXPECT_EQ(true, MediaSessionMap["audio"].Protocol == "RTP/AVP");
	EXPECT_EQ(true, MediaSessionMap["audio"].TimeRate == 90000);

	EXPECT_EQ(1, MediaSessionMap["audio"].PayloadType.size());
	for(vector<int>::iterator it = MediaSessionMap["audio"].PayloadType.begin(); it != MediaSessionMap["audio"].PayloadType.end(); it++) {
		EXPECT_EQ(14, *it);
	}
}
