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

#include "rtspClient.h"
#include "utils.h"
#include "Base64.hh"
#include "nalu_types_h264.h"
#include "mpeg_types.h"

#include <sstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define GET_SPS_PPS_PERIOD 	30

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;

extern STAP_A 		STAP_AObj;
extern STAP_B 		STAP_BObj;
extern MTAP_16 		MTAP_16Obj;
extern MTAP_24 		MTAP_24Obj;
extern FU_A 		FU_AObj;
extern FU_B 		FU_BObj;
extern NALUTypeBase_H264 NaluBaseType_H264Obj;

extern MPEG_Audio MPEG_AudioObj;

RtspClient::RtspClient():
	RtspURI(""), RtspCSeq(0), RtspSockfd(-1), RtspIP(""), RtspPort(PORT_RTSP), RtspResponse(""), SDPStr(""), 
	SPS(""), PPS(""), CmdPLAYSent(false), GetVideoDataCount(GET_SPS_PPS_PERIOD)
{
	// SDPInfo = new multimap<string, string>;
	MediaSessionMap = new map<string, MediaSession>;

	/* Temporary only FU_A supported */
	// NALUType = new FU_A;
}

RtspClient::RtspClient(string uri):
	RtspURI(uri), RtspCSeq(0), RtspSockfd(-1), RtspIP(""), RtspPort(PORT_RTSP), RtspResponse(""), SDPStr(""),
	SPS(""), PPS(""), CmdPLAYSent(false), GetVideoDataCount(GET_SPS_PPS_PERIOD)
{
	// SDPInfo = new multimap<string, string>;
	MediaSessionMap = new map<string, MediaSession>;

	/* Temporary only FU_A supported */
	// NALUType = new FU_A;
}

RtspClient::~RtspClient()
{
	// delete SDPInfo;
	delete MediaSessionMap;
	MediaSessionMap = NULL;

	// delete NALUType;
	// NALUType = NULL;
}

ErrorType RtspClient::DoDESCRIBE(string uri)
{
	string RtspUri("");
	int Sockfd = -1;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return RTSP_INVALID_URI;

	Sockfd = CreateTcpSockfd(RtspUri);
	if(Sockfd < 0) return RTSP_INVALID_URI;

	string Cmd("DESCRIBE");
	stringstream Msg("");
	Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(!SendRTSP(Sockfd, Msg.str())) {
		close(Sockfd);
		return RTSP_SEND_ERROR;
	}
	if(!RecvRTSP(Sockfd, &RtspResponse)) {
		close(Sockfd);
		return RTSP_RECV_ERROR;
	}
	RecvSDP(Sockfd, &SDPStr);
	close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoOPTIONS(string uri)
{
	string RtspUri("");
	int Sockfd = -1;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return RTSP_INVALID_URI;

	Sockfd = CreateTcpSockfd(RtspUri);
	if(Sockfd < 0) return RTSP_INVALID_URI;

	string Cmd("OPTIONS");
	stringstream Msg("");
	Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(!SendRTSP(Sockfd, Msg.str())) {
		close(Sockfd);
		return RTSP_SEND_ERROR;
	}
	if(!RecvRTSP(Sockfd, &RtspResponse)) {
		close(Sockfd);
		return RTSP_RECV_ERROR;
	}
	close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoPAUSE()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoPAUSE(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("PAUSE Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoPAUSE(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	string Cmd("PAUSE");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoPAUSE(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoPAUSE(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoSETUP()
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoSETUP(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("Setup Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoSETUP(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	// "CreateUdpSockfd" is only for test. 
	// We will use jrtplib instead later. 
	if(!SetAvailableRTPPort(media_session)) {
		printf("No port available for RTP and RTCP\n");
		return RTSP_UNKNOWN_ERROR;
	}
	string Cmd("SETUP");
	stringstream Msg("");
	Msg << Cmd << " " << media_session->ControlURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "Transport:" << " " << media_session->Protocol << "/UDP;";
	Msg << "unicast;" << "client_port=" << media_session->RTPPort << "-" << media_session->RTCPPort << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	media_session->SessionID = ParseSessionID(RtspResponse);
	media_session->RTP_SetUp();

	// media_session->RTSPSockfd = Sockfd;
	close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoSETUP(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;


	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoSETUP(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoPLAY()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoPLAY(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("PLAY Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoPLAY(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}

	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;
	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;

	string Cmd("PLAY");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoPLAY(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoPLAY(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoTEARDOWN()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	Err = DoTEARDOWN("audio");
	if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
	Err = DoTEARDOWN("video");
	if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error

	// for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
	// 	cerr << "TEST: Ready to teardown: " << it->first << endl;
	// 	Err = DoTEARDOWN(&(it->second));
	// 	if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
	// 	printf("TEARDOWN Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	// }

	return ErrAll;
}

ErrorType RtspClient::DoTEARDOWN(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	cout << "TEST: TEARDOWN: ###" << media_session->MediaType << "###" << endl;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	string Cmd("TEARDOWN");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	if(RTSP_NO_ERROR == Err) {
		map<string, MediaSession>::iterator it;
		for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
			if(media_session->SessionID == it->second.SessionID) break;
		}
		if(it != MediaSessionMap->end()) {
			MediaSessionMap->erase(it);
			// close(media_session->RTPSockfd);
			// close(media_session->RTCPSockfd);
		}
	}
	close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoTEARDOWN(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoTEARDOWN(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

int RtspClient::ParseSDP(string SDP)
{
	MyRegex Regex;
	string Response("");
	int Result = 0; // don't have meaning yet

	if(SDP.length() != 0) Response.assign(SDP);
	else if(RtspResponse.length() != 0) Response.assign(SDPStr);
	else return Result;

	string Pattern = "([a-zA-Z])=(.*)";
	list<string> Group;
	bool CollectMediaInfo = false;
	string CurrentMediaSession("");
	while(Regex.RegexLine(&Response, &Pattern, &Group)) {
		string Key(""), Value("");
		if(!Group.empty()) {
			Group.pop_front(); // pop the line
			Group.pop_front(); // pop the matched part
			Key.assign(Group.front()); Group.pop_front();
			Value.assign(Group.front()); Group.pop_front();
			// SDPInfo->insert(pair<string, string>(Key, Value));
		}
		if(Key == "m") CollectMediaInfo = true;
		if(Key == "s") CollectMediaInfo = false;
		if(!CollectMediaInfo) continue;

		if(Key == "m") {
			string PatternTmp("([a-zA-Z]+) +.+ +(.+) +.*");
			if(!Regex.Regex(Value.c_str(), PatternTmp.c_str(), &Group)) {
				continue;
			}
			Group.pop_front();
			CurrentMediaSession.assign(Group.front());
			Group.pop_front();
			string Protocol(Group.front());
			MediaSession NewMediaSession;
			NewMediaSession.MediaType.assign(CurrentMediaSession);
			NewMediaSession.Protocol.assign(Protocol);
			(*MediaSessionMap)[CurrentMediaSession] = NewMediaSession;

		}
		if("a" == Key) {
			string PatternRtpmap("rtpmap:.* +([0-9A-Za-z]+)/([0-9]+)");
			string PatternFmtp("fmtp:.*sprop-parameter-sets=([A-Za-z0-9+/=]+),([A-Za-z0-9+/=]+)");
			string PatternControl("control:(.+)");
			if(CurrentMediaSession.length() == 0) {
				continue;
			}
			if(Regex.Regex(Value.c_str(), PatternRtpmap.c_str(), &Group)) {
				Group.pop_front();
				(*MediaSessionMap)[CurrentMediaSession].EncodeType = Group.front();;
				Group.pop_front();
				stringstream TimeRate;
				TimeRate << Group.front();
				TimeRate >> (*MediaSessionMap)[CurrentMediaSession].TimeRate;
			} else if(Regex.Regex(Value.c_str(), PatternControl.c_str(), &Group)) {
				Group.pop_front();
				string ControlURITmp("");
				/* 'Value' could be  
				 * 1: "rtsp://127.0.0.1/ansersion/track=1"
				 * 2: "track=1"
				 * If is the '2', it should be prefixed with the URI. */
				if(!Regex.Regex(Value.c_str(), "rtsp://")) {
					ControlURITmp += RtspURI;
					ControlURITmp += "/";
				}
				ControlURITmp += Group.front();
				printf("Control: %s\n", ControlURITmp.c_str());
				(*MediaSessionMap)[CurrentMediaSession].ControlURI.assign(ControlURITmp);
			} else if(Regex.Regex(Value.c_str(), PatternFmtp.c_str(), &Group)) {
				Group.pop_front();
				SPS.assign(Group.front());
				Group.pop_front();
				PPS.assign(Group.front());

				if(Regex.Regex(Value.c_str(), "packetization-mode=([0-2])", &Group)) {
					Group.pop_front();
					stringstream PacketizationMode;
					PacketizationMode << Group.front();
					PacketizationMode >> (*MediaSessionMap)[CurrentMediaSession].Packetization;
				}
			}
		}
	}

	return Result;
}

string RtspClient::ParseError(ErrorType et)
{
	string ErrStr("");
	switch(et) {
		case RTSP_NO_ERROR:
			ErrStr.assign("MyRtsp: Success");
			break;
		case RTSP_INVALID_URI:
			ErrStr.assign("MyRtsp: Invalid URI");
			break;
		case RTSP_SEND_ERROR:
			ErrStr.assign("MyRtsp: send error");
			break;
		case RTSP_RECV_ERROR:
			ErrStr.assign("MyRtsp: recv error");
			break;
		case RTSP_INVALID_MEDIA_SESSION:
			ErrStr.assign("MyRtsp: invalid media session error");
			break;
		case RTSP_RESPONSE_BLANK:
			ErrStr.assign("MyRtsp: Response BLANK");
			break;
		case RTSP_RESPONSE_200:
			ErrStr.assign("MyRtsp: Response 200 OK");
			break;
		case RTSP_RESPONSE_400:
			ErrStr.assign("MyRtsp: Response 400 Bad Request");
			break;
		case RTSP_RESPONSE_401:
			ErrStr.assign("MyRtsp: Response 401 Unauthorized");
			break;
		case RTSP_RESPONSE_404:
			ErrStr.assign("MyRtsp: Response 404 Not Found");
			break;
		case RTSP_RESPONSE_40X:
			ErrStr.assign("MyRtsp: Response Client Error");
			break;
		case RTSP_RESPONSE_500:
			ErrStr.assign("MyRtsp: Response 500 Internal Server Error");
			break;
		case RTSP_RESPONSE_501:
			ErrStr.assign("MyRtsp: Response 501 Not Implemented");
			break;
		case RTSP_RESPONSE_50X:
			ErrStr.assign("MyRtsp: Response Server Error");
			break;
		case RTSP_UNKNOWN_ERROR:
			ErrStr.assign("MyRtsp: Unknown Error");
			break;
		default:
			ErrStr.assign("MyRtsp: Unknown Error");
			break;
	}
	return ErrStr;
}

/*****************/
/* Tools Methods */
int RtspClient::CreateTcpSockfd(string uri)
{
	int Sockfd = -1;
	struct sockaddr_in Servaddr;
	string RtspUri("");
	int SockStatus = -1;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return Sockfd;

	if((Sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket created error");
		return Sockfd;
	}

	// Set Sockfd NONBLOCK
	SockStatus = fcntl(Sockfd, F_GETFL, 0);
	fcntl(Sockfd, F_SETFL, SockStatus | O_NONBLOCK);

	// Connect to server
	bzero(&Servaddr, sizeof(Servaddr));
	Servaddr.sin_family = AF_INET;
	Servaddr.sin_port = htons(GetPort(uri));
	Servaddr.sin_addr.s_addr = GetIP(uri);

	if(connect(Sockfd, (struct sockaddr *)&Servaddr, sizeof(Servaddr)) < 0 && errno != EINPROGRESS) {
		perror("connect error");
		close(Sockfd);
		Sockfd = -1;
		return Sockfd;
	}
	if(!CheckSockWritable(Sockfd)) {
		Sockfd = -1;
		return Sockfd;
	}

	// RtspSockfd = Sockfd;
	return Sockfd;
}

int RtspClient::SetAvailableRTPPort(MediaSession * media_session, uint16_t RTP_port)
{
	int RTPSockfd, RTCPSockfd;
	uint16_t RTPPort;
	uint16_t RTCPPort;
	struct sockaddr_in servaddr;
	uint16_t Search_RTP_Port_From;
	if(0 != RTP_port && (RTP_port % 2 == 0)) {
		Search_RTP_Port_From = RTP_port;
	} else {
		Search_RTP_Port_From = SEARCH_PORT_RTP_FROM;
	}
	media_session->RTPPort = 0;
	// media_session->RTPSockfd = -1;
	media_session->RTCPPort = 0;
	// media_session->RTCPSockfd = -1;

	// Create RTP and RTCP udp socket 
	for(RTPPort = Search_RTP_Port_From; RTPPort < 65535; RTPPort = RTPPort + 2) {
		// Bind RTP Port
		if((RTPSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("CreateRTP_RTCPSockfd Error");
			return 0;
		}	
		// if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) < 0) { 
		// 	close(RTPPort);
		// 	perror("CreateRTP_RTCPSockfd Error");
		// 	return 0;
		// } 
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(RTPPort);
		if(bind(RTPSockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
			close(RTPSockfd);
			continue;
		}

		// Bind RTCP Port
		RTCPPort = RTPPort + 1;
		if((RTCPSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			close(RTPSockfd);
			perror("CreateRTP_RTCPSockfd Error");
			return 0; // Create failed
		}	
		// if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) < 0) { 
		// 	close(RTPPort);
		// 	close(RTCPPort);
		// 	perror("CreateRTP_RTCPSockfd Error");
		// 	return 0;
		// } 
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(RTCPPort);

		if(bind(RTCPSockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
			close(RTPSockfd);
			close(RTCPSockfd);
			continue;
		}
		close(RTPSockfd);
		close(RTCPSockfd);
		media_session->RTPPort = RTPPort;
		// media_session->RTPSockfd = RTPSockfd;;
		media_session->RTCPPort = RTCPPort;
		// media_session->RTCPSockfd = RTCPSockfd;
		return 1; // Created successfully
	}
	return 0; // Created failed
}

in_addr_t RtspClient::GetIP(string uri)
{
	//### example uri: rtsp://192.168.15.100/test ###//
	MyRegex Regex;
	string RtspUri("");
	string Pattern("rtsp://([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})\\b");
	list<string> Groups;
	bool IgnoreCase = true;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return inet_addr(RtspIP.c_str());

	if(!Regex.Regex(RtspUri.c_str(), Pattern.c_str(), &Groups, IgnoreCase)) {
		return INADDR_NONE;
	}
	Groups.pop_front();
	return inet_addr(Groups.front().c_str());
}

uint16_t RtspClient::GetPort(string uri)
{
	//### example uri: rtsp://192.168.15.100:554/test ###//
	MyRegex Regex;
	string RtspUri("");
	string Pattern("rtsp://[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}:([0-9]+)");
	list<string> Groups;
	bool IgnoreCase = true;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return RtspPort;

	if(!Regex.Regex(RtspUri.c_str(), Pattern.c_str(), &Groups, IgnoreCase)) {
		return RtspPort;
	}
	Groups.pop_front();
	return atoi(Groups.front().c_str());
}

/*********************/
/* Protected Methods */
int RtspClient::CheckSockWritable(int sockfd, struct timeval * tval)
{
	fd_set Wset;
	struct timeval Tval;
	FD_ZERO(&Wset);
	FD_SET(sockfd, &Wset);
	if(!tval) {
		Tval.tv_sec = SELECT_TIMEOUT_SEC;
		Tval.tv_usec = SELECT_TIMEOUT_USEC;
	} else {
		Tval = *tval;
	}

	while(select(sockfd + 1, NULL, &Wset, NULL, &Tval) != 0) {
		if(FD_ISSET(sockfd, &Wset)) {
			return CHECK_OK;
		}
		return CHECK_ERROR;
	}   
	printf("Select Timeout\n");
	return CHECK_ERROR;
}

int RtspClient::CheckSockReadable(int sockfd, struct timeval * tval)
{
	fd_set Rset;
	struct timeval Tval;
	FD_ZERO(&Rset);
	FD_SET(sockfd, &Rset);
	if(!tval) {
		Tval.tv_sec = SELECT_TIMEOUT_SEC;
		Tval.tv_usec = SELECT_TIMEOUT_USEC;
	} else {
		Tval = *tval;
	}

	while(select(sockfd + 1, &Rset, NULL, NULL, &Tval) != 0) {
		if(FD_ISSET(sockfd, &Rset)) {
			return CHECK_OK;
		}
		return CHECK_ERROR;
	}   
	printf("Select Timeout\n");
	return CHECK_ERROR;
}

int RtspClient::SendRTSP(int fd, const char * msg, size_t size)
{
 	if(!msg || size < 0) {
 		printf("Recv Argument Error\n");
 		return TRANS_ERROR;
 	}

 	int SendResult = 0;
 	int Index = 0;
 	int Err = TRANS_OK;

	while(size > 0) {
		if(!CheckSockWritable(fd)) {
			Err = TRANS_ERROR;
			break;
		}
		SendResult = Writen(fd, msg+Index, size);
		if(SendResult < 0) {
			if(errno == EINTR) continue;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) continue;
			else {
				Err = TRANS_ERROR;
				break;
			}
		} else if(SendResult == 0) {
			Err = TRANS_ERROR;
			break;
		}
		Index += SendResult;
		size -= SendResult;
	}

	return Err;
}
// {
// 	if(!msg || size <= 0) {
// 		printf("Recv Argument Error\n");
// 		return TRANS_ERROR;
// 	}
// 
// 	int SendResult = 0;
// 	int Index = 0;
// 	int Err = TRANS_OK;
// 	while(size != 0) {
// 		if(!CheckSockWritable(RtspSockfd)) {
// 			Err = TRANS_ERROR;
// 			break;
// 		}
// 		SendResult = send(RtspSockfd, msg+Index, size, 0);
// 		if(0 == SendResult) {
// 			printf("Socket disconnected\n");
// 			Err = TRANS_ERROR;
// 			break;
// 		} else if(SendResult < 0) {
// 			if(errno == EINTR) continue;
// 			else if(errno == EWOULDBLOCK || errno == EAGAIN) {
// 				Err = TRANS_OK;
// 				break;
// 			} else {
// 				Err = TRANS_ERROR;
// 				break;
// 			}
// 		} else {
// 			Index += SendResult;
// 			size -= SendResult;
// 			continue;
// 		}
// 	}
// 
// 	return Err;
// }

int RtspClient::SendRTSP(int fd, string msg)
{
	return SendRTSP(fd, msg.c_str(), msg.length());
}

int RtspClient::RecvRTSP(int fd, char * msg, size_t maxlen)
{
	MyRegex Regex;
	if(!msg || maxlen < 0) {
		printf("Recv Argument Error\n");
		return TRANS_ERROR;
	}

	int RecvResult = 0;
	int Index = 0;
	int Err = TRANS_OK;

	memset(msg, 0, maxlen);
	while(maxlen > 0) {
		if(!CheckSockReadable(fd)) {
			Err = TRANS_ERROR;
			break;
		}
		RecvResult = ReadLine(fd, msg + Index, maxlen);
		if(RecvResult < 0) {
			if(errno == EINTR) continue;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) {
				Err = TRANS_OK;
				break;
			} else {
				Err = TRANS_ERROR;
				break;
			}
		} else if(RecvResult == 0) {
			Err = TRANS_ERROR;
			break;
		}
		/*
		 * Single with "\r\n" or "\n" is the termination tag of RTSP.
		 * */
		if(RecvResult <= (int)strlen("\r\n") &&
				Regex.Regex(msg+Index, "^(\r\n|\n)$")) {
			Err = TRANS_OK;
			break;
		}
		Index += RecvResult;
		maxlen -= RecvResult;
	}
	return Err;
}

int RtspClient::RecvRTSP(int fd, string * msg)
{
	if(!msg) {
		printf("Invalid Argument\n");
		return TRANS_ERROR;
	}

	char * Buf = (char *)calloc(RECV_BUF_SIZE, sizeof(char));
	int RecvResult = TRANS_OK;

	msg->assign("");
	RecvResult = RecvRTSP(fd, Buf, RECV_BUF_SIZE);
	if(TRANS_OK == RecvResult) msg->assign(Buf);
	free(Buf);
	return RecvResult;
}

int RtspClient::RecvSDP(int sockfd, char * msg, size_t size)
{
	if(!msg || size < 0) {
		printf("Recv Argument Error\n");
		return TRANS_ERROR;
	}

	int RecvResult = 0;
	int Index = 0;
	int Err = TRANS_OK;

	memset(msg, 0, size);
	while(size > 0) {
		if(!CheckSockReadable(sockfd)) {
			Err = TRANS_ERROR;
			break;
		}
		RecvResult = Readn(sockfd, msg + Index, size);
		if(RecvResult < 0) {
			if(errno == EINTR) continue;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) {
				Err = TRANS_OK;
				break;
			} else {
				Err = TRANS_ERROR;
				break;
			}
		} else if(RecvResult == 0) {
			Err = TRANS_ERROR;
			break;
		}
		Index += RecvResult;
		size -= RecvResult;
	}

	return Err;
}

int RtspClient::RecvSDP(int sockfd, string * msg)
{
	MyRegex Regex;
	if(!msg) msg = &SDPStr;

	char * Buf = (char *)calloc(RECV_BUF_SIZE, sizeof(char));
	size_t Size = 0;
	int RecvResult = TRANS_OK;
	string DESCRIBEResponse(RtspResponse);

	msg->assign("");
	string Pattern("Content-Length: +([0-9]+)");
	list<string> Group;
	stringstream tmp;
	if(Regex.Regex(DESCRIBEResponse.c_str(), Pattern.c_str(), &Group)) {
		Group.pop_front();
		tmp << Group.front();
		tmp >> Size;
		if(Size >= RECV_BUF_SIZE) {
			printf("SDP size:Too large\n");
			return TRANS_ERROR;
		}
	} else {
		// Invalid "DESCRIBE" Response
		return TRANS_ERROR;
	}
	RecvResult = RecvSDP(sockfd, Buf, Size);
	if(TRANS_OK == RecvResult) msg->assign(Buf);
	free(Buf);
	return RecvResult;
}

int RtspClient::Close(int sockfd)
{
	int CloseResult = -1;
	if(sockfd == RtspSockfd) {
		CloseResult = close(RtspSockfd);
		RtspSockfd = -1;
	} else {
		if(sockfd > 0) CloseResult = close(sockfd);
		if(RtspSockfd > 0) CloseResult = close(RtspSockfd);
	}
	return CloseResult;
}

string RtspClient::ParseSessionID(string ResponseOfSETUP)
{
	MyRegex Regex;
	string Response("");
	string Result("");

	if(ResponseOfSETUP.length() != 0) Response.assign(ResponseOfSETUP);
	else if(RtspResponse.length() != 0) Response.assign(RtspResponse);
	else return Result;

	// Session: 970756dc30b3a638;timeout=60
	string Pattern("Session: +(\\w+)");
	list<string> Group;
	bool IgnoreCase = true;
	if(Regex.Regex(Response.c_str(), Pattern.c_str(), &Group, IgnoreCase)) {
		Group.pop_front();
		Result.assign(Group.front());
	}
	return Result;
}

bool RtspClient::IsResponse_200_OK(ErrorType * err, string * response)
{
	// example: 
	// "RTSP/1.0 200 OK"

	MyRegex Regex;
	string Pattern200("RTSP/[0-9]+\\.[0-9]+ +200");
	string Pattern40x("RTSP/[0-9]+\\.[0-9]+ +40[0-9]");
	string Pattern50x("RTSP/[0-9]+\\.[0-9]+ +50[0-9]");

	list<string> Groups;
	bool IgnoreCase = true;
	bool Result = false;

	string Response("");

	if(!response) Response.assign(RtspResponse);
	else Response.assign(*response);

	if(Regex.Regex(Response.c_str(), Pattern200.c_str(), &Groups, IgnoreCase)) {
		Result = true;
		if(!err) return Result;
		*err = RTSP_RESPONSE_200;
	} else if(Regex.Regex(Response.c_str(), Pattern40x.c_str(), &Groups, IgnoreCase)) {
		Result = false;
		if(!err) return Result;
		*err = RTSP_RESPONSE_40X;
	} else if(Regex.Regex(Response.c_str(), Pattern50x.c_str(), &Groups, IgnoreCase)) {
		Result = false;
		if(!err) return Result;
		*err = RTSP_RESPONSE_50X;
	}
	return Result;
}

uint8_t * RtspClient::GetMediaData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size) 
{
	if(!media_session) return NULL;
	return media_session->GetMediaData(buf, size, max_size);
}

uint8_t * RtspClient::GetMediaData(string media_type, uint8_t * buf, size_t * size, size_t max_size) 
{
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it == MediaSessionMap->end()) {
		fprintf(stderr, "%s: No such media session\n", __func__);
		return NULL;
	}

	if(it->second.MediaType == "video") return GetVideoData(&(it->second), buf, size, max_size);
	if(it->second.MediaType == "audio") return GetAudioData(&(it->second), buf, size, max_size);
	return NULL;
}

uint8_t * RtspClient::GetVideoData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size, bool get_sps_pps_periodly) 
{
	if(!media_session || !buf || !size) return NULL;

	*size = 0;

	const size_t GetSPS_PPS_Period = GET_SPS_PPS_PERIOD; // 30 times
	if(true == get_sps_pps_periodly) {
		if(GetVideoDataCount >= GetSPS_PPS_Period) {
			GetVideoDataCount = 0;

			const size_t NALU_StartCodeSize = 4;
			size_t SizeTmp = 0;
			if(!GetSPSNalu(buf + (*size), &SizeTmp) || SizeTmp <= NALU_StartCodeSize) {
				fprintf(stderr, "\033[31mWARNING: No H264 SPS\033[0m\n");
			} else {
				*size += SizeTmp;
			}
			if(!GetPPSNalu(buf + (*size), &SizeTmp) || SizeTmp <= NALU_StartCodeSize) {
				fprintf(stderr, "\033[31mWARNING: No H264 PPS\033[0m\n");
			} else {
				*size += SizeTmp;
			}
			return buf;
		} else {
			GetVideoDataCount++;
		}
	}

	size_t SizeTmp = 0;
	bool EndFlag = false;
    NALUTypeBase * NALUType;

	int PM = media_session->Packetization;
	if(!IS_PACKET_MODE_VALID(PM)) {
		cerr << "WARNING:Invalid Packetization Mode" << endl;
		return NULL;
	}

	do {
		EndFlag = true;
		if(!media_session->GetMediaData(VideoBuffer, &SizeTmp)) return NULL;
		if(0 == SizeTmp) {
			cerr << "No RTP data" << endl;
			return NULL;
		}
		int NT = NaluBaseType_H264Obj.ParseNALUHeader_Type(VideoBuffer);
		if(!IS_NALU_TYPE_VALID(NT)) {
			cerr << "WARNING:Invalid NALU" << endl;
			return NULL;
		}

        // if((NALUType = NALUType->NalUnitType[PM][NT]) == NULL) {
        if((NALUType = NALUTypeBase::NalUnitType_H264[PM][NT]) == NULL) {
            cerr << "Error: Unsupported RTP H264 payload type!" << endl;
            return NULL;
        }

        if(SizeTmp > sizeof(VideoBuffer)) {
            cerr << "Error: RTP Packet too large(" << SizeTmp << " bytes > " << sizeof(VideoBuffer) << "bytes)" << endl;
            return NULL;
        }

		if(*size + SizeTmp > max_size) {
			fprintf(stderr, "\033[31mWARNING: NALU truncated because larger than buffer: %u(NALU size) > %u(Buffer size)\033[0m\n", *size + SizeTmp, max_size);
			return buf;
		}

		SizeTmp = NALUType->CopyData(buf + (*size), VideoBuffer, SizeTmp);
		*size += SizeTmp;
		EndFlag = NALUType->GetEndFlag();
	} while(!EndFlag);

	return buf;
}

uint8_t * RtspClient::GetAudioData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size)
{
	if(!media_session || !buf || !size) return NULL;

	*size = 0;

	size_t SizeTmp = 0;
	MPEGTypeBase * MPEGType;

	if(!media_session->GetMediaData(AudioBuffer, &SizeTmp)) return NULL;
	if(0 == SizeTmp) {
		cerr << "No RTP data" << endl;
		return NULL;
	}

	MPEGType = &MPEG_AudioObj;

	if(SizeTmp > sizeof(AudioBuffer)) {
		cerr << "Error: RTP Packet too large(" << SizeTmp << " bytes > " << sizeof(AudioBuffer) << "bytes)" << endl;
		return NULL;
	}

	if(*size + SizeTmp > max_size) {
		fprintf(stderr, "\033[31mWARNING: NALU truncated because larger than buffer: %u(NALU size) > %u(Buffer size)\033[0m\n", *size + SizeTmp, max_size);
		return buf;
	}

	SizeTmp = MPEGType->CopyData(buf + (*size), AudioBuffer, SizeTmp);
	*size += SizeTmp;

	return buf;
}

uint8_t * RtspClient::GetMediaPacket(MediaSession * media_session, uint8_t * buf, size_t * size) 
{
	if(!media_session) return NULL;
	return media_session->GetMediaPacket(buf, size);
}

uint8_t * RtspClient::GetMediaPacket(string media_type, uint8_t * buf, size_t * size) {
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it == MediaSessionMap->end()) {
		fprintf(stderr, "%s: No such media session\n", __func__);
		return NULL;
	}

	return it->second.GetMediaPacket(buf, size);
}

uint8_t * RtspClient::GetSPSNalu(uint8_t * buf, size_t * size)
{
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	*size += 4;

	unsigned int SpsSize = 0;
	unsigned char * Sps = base64Decode(SPS.c_str(), SpsSize, false);
	memcpy(buf + (*size), Sps, SpsSize);
	*size += SpsSize;
	delete[] Sps;
	Sps = NULL;

	return buf;

}

uint8_t * RtspClient::GetPPSNalu(uint8_t * buf, size_t * size)
{
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	*size += 4;

	unsigned int PpsSize = 0;
	unsigned char * Pps = base64Decode(PPS.c_str(), PpsSize, false);
	memcpy(buf + (*size), Pps, PpsSize);
	*size += PpsSize;
	delete[] Pps;
	Pps = NULL;

	return buf;
}

