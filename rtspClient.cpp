#include "rtspClient.h"
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

using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::pair;

RtspClient::RtspClient():
	RtspURI(""), RtspCSeq(0), RtspSockfd(-1), RtspIP(""), RtspPort(PORT_RTSP), RtspResponse("")
{
	SDPInfo.clear();
	MediaSessionMap.clear();
}

RtspClient::RtspClient(string uri):
	RtspURI(uri), RtspCSeq(0), RtspSockfd(-1), RtspIP(""), RtspPort(PORT_RTSP), RtspResponse("")
{
	SDPInfo.clear();
	MediaSessionMap.clear();
}

RtspClient::~RtspClient()
{


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

	string Cmd("DESCRIBE");
	stringstream Msg("");
	Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(!Send(Msg.str())) {
		Close(Sockfd);
		return RTSP_SEND_ERROR;
	}
	if(!Recv(&RtspResponse)) {
		Close(Sockfd);
		return RTSP_RECV_ERROR;
	}
	Close(Sockfd);
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

	string Cmd("OPTIONS");
	stringstream Msg("");
	Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(!Send(Msg.str())) {
		Close(Sockfd);
		return RTSP_SEND_ERROR;
	}
	if(!Recv(&RtspResponse)) {
		Close(Sockfd);
		return RTSP_RECV_ERROR;
	}
	Close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoPAUSE()
{
}

ErrorType RtspClient::DoPLAY()
{
}

ErrorType RtspClient::DoSETUP(MediaSession * media_session)
{
	// map<string , MediaSession> MediaSessionToSetup;
	// int Sockfd = -1;

	// if(!media_session) {
	// 	MediaSessionToSetup = GetMediaSessions();
	// } else {
	// 	MediaSessionToSetup.insert(pair<string, MediaSession>(media_session->MediaType, media_session));
	// }

	// Sockfd = CreateTcpSockfd(RtspUri);

	// string Cmd("OPTIONS");
	// stringstream Msg("");
	// Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	// Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	// Msg << "\r\n";

	// if(!Send(Msg.str())) {
	// 	Close(Sockfd);
	// 	return RTSP_SEND_ERROR;
	// }
	// if(!Recv(&RtspResponse)) {
	// 	Close(Sockfd);
	// 	return RTSP_RECV_ERROR;
	// }
	// can't close.
	// Socket must be active after SETUP.
	// Close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoTEARDOWN()
{
}

int RtspClient::ParseSDP(string uri)
{
	string Response("");
	int Result = 0; // don't have meaning yet

	if(uri.length() != 0) Response.assign(uri);
	else if(RtspResponse.length() != 0) Response.assign(RtspResponse);
	else return Result;

	string Pattern = "([a-zA-Z])=(.*)";
	list<string> Group;
	int i = 0;
	bool CollectMediaInfo = false;
	string CurrentMediaSession("");
	while(Regex.RegexLine(&Response, &Pattern, &Group)) {
		string Key(""), Value("");
		if(!Group.empty()) {
			Group.pop_front(); // pop the line
			Group.pop_front(); // pop the matched part
			Key.assign(Group.front()); Group.pop_front();
			Value.assign(Group.front()); Group.pop_front();
			SDPInfo.insert(pair<string, string>(Key, Value));
		}
		if(Key == "m") CollectMediaInfo = true;
		if(Key == "s") CollectMediaInfo = false;
		if(!CollectMediaInfo) continue;

		if(Key == "m") {
			string PatternTmp("([a-zA-Z]+) +");
			if(!Regex.Regex(Value.c_str(), PatternTmp.c_str(), &Group)) {
				continue;
			}
			Group.pop_front();
			CurrentMediaSession.assign(Group.front());
			MediaSession NewMediaSession;
			NewMediaSession.MediaType.assign(CurrentMediaSession);
			MediaSessionMap[CurrentMediaSession] = NewMediaSession;
		}
		if("a" == Key) {
			string PatternRtpmap("rtpmap:.* +([0-9A-Za-z]+)/([0-9]+)");
			string PatternControl("control:(.+)");
			if(CurrentMediaSession.length() == 0) {
				continue;
			}
			if(Regex.Regex(Value.c_str(), PatternRtpmap.c_str(), &Group)) {
				Group.pop_front();
				MediaSessionMap[CurrentMediaSession].EncodeType = Group.front();;
				Group.pop_front();
				stringstream TimeRate;
				TimeRate << Group.front();
				TimeRate >> MediaSessionMap[CurrentMediaSession].TimeRate;
			} else if(Regex.Regex(Value.c_str(), PatternControl.c_str(), &Group)) {
				Group.pop_front();
				MediaSessionMap[CurrentMediaSession].ControlURI = Group.front();;
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
			ErrStr.assign("MyRtsp Success");
			break;
		case RTSP_INVALID_URI:
			ErrStr.assign("MyRtsp Invalid URI");
			break;
		case RTSP_SEND_ERROR:
			ErrStr.assign("MyRtsp send error");
			break;
		case RTSP_RECV_ERROR:
			ErrStr.assign("MyRtsp recv error");
			break;
		case RTSP_RESPONSE_BLANK:
			ErrStr.assign("MyRtsp Response BLANK");
			break;
		case RTSP_RESPONSE_200:
			ErrStr.assign("MyRtsp Response 200 OK");
			break;
		case RTSP_RESPONSE_400:
			ErrStr.assign("MyRtsp Response 400 Bad Request");
			break;
		case RTSP_RESPONSE_401:
			ErrStr.assign("MyRtsp Response 401 Unauthorized");
			break;
		case RTSP_RESPONSE_404:
			ErrStr.assign("MyRtsp Response 404 Not Found");
			break;
		case RTSP_RESPONSE_500:
			ErrStr.assign("MyRtsp Response 500 Internal Server Error");
			break;
		case RTSP_RESPONSE_501:
			ErrStr.assign("MyRtsp Response 501 Not Implemented");
			break;
		case RTSP_UNKNOWN_ERROR:
			ErrStr.assign("MyRtsp Unknown Error");
			break;
		default:
			ErrStr.assign("MyRtsp Unknown Error");
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
	in_addr_t Ip = 0;
	uint16_t Port = 0;
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

	RtspSockfd = Sockfd;
	return Sockfd;
}

// int RtspClient::CreateUdpSockfd(uint16_t RTPPort)
// {
// 	int Sockfd = -1;
// 	struct sockaddr_in Servaddr;
// 	in_addr_t Ip = 0;
// 	uint16_t Port = 0;
// 	string RtspUri("");
// 	int SockStatus = -1;
// 
// 	if(uri.length() != 0) {
// 		RtspUri.assign(uri);
// 		RtspURI.assign(uri);
// 	}
// 	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
// 	else return Sockfd;
// 
// 	if((Sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
// 		perror("Socket created error");
// 		return Sockfd;
// 	}
// 
// 	// Set Sockfd NONBLOCK
// 	SockStatus = fcntl(Sockfd, F_GETFL, 0);
// 	fcntl(Sockfd, F_SETFL, SockStatus | O_NONBLOCK);
// 
// 	// Connect to server
// 	bzero(&Servaddr, sizeof(Servaddr));
// 	Servaddr.sin_family = AF_INET;
// 	Servaddr.sin_port = htons(GetPort(uri));
// 	Servaddr.sin_addr.s_addr = GetIP(uri);
// 
// 	if(connect(Sockfd, (struct sockaddr *)&Servaddr, sizeof(Servaddr)) < 0 && errno != EINPROGRESS) {
// 		perror("connect error");
// 		close(Sockfd);
// 		Sockfd = -1;
// 		return Sockfd;
// 	}
// 	if(!CheckSockWritable(Sockfd)) {
// 		Sockfd = -1;
// 		return Sockfd;
// 	}
// 
// 	RtspSockfd = Sockfd;
// 	return Sockfd;
// }

in_addr_t RtspClient::GetIP(string uri)
{
	//### example uri: rtsp://192.168.15.100/test ###//
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
int RtspClient::CheckSockWritable(int sockfd)
{
	fd_set Wset;
	struct timeval Tval;
	FD_ZERO(&Wset);
	FD_SET(sockfd, &Wset);
	Tval.tv_sec = SELECT_TIMEOUT_SEC;
	Tval.tv_usec = SELECT_TIMEOUT_USEC;

	while(select(sockfd + 1, NULL, &Wset, NULL, &Tval) != 0) {
		if(FD_ISSET(sockfd, &Wset)) {
			return CHECK_OK;
		}
		return CHECK_ERROR;
	}   
	printf("Select Timeout\n");
	return CHECK_ERROR;
}

int RtspClient::CheckSockReadable(int sockfd)
{
	fd_set Rset;
	struct timeval Tval;
	FD_ZERO(&Rset);
	FD_SET(sockfd, &Rset);
	Tval.tv_sec = SELECT_TIMEOUT_SEC;
	Tval.tv_usec = SELECT_TIMEOUT_USEC;

	while(select(sockfd + 1, &Rset, NULL, NULL, &Tval) != 0) {
		if(FD_ISSET(sockfd, &Rset)) {
			return CHECK_OK;
		}
		return CHECK_ERROR;
	}   
	printf("Select Timeout\n");
	return CHECK_ERROR;
}

int RtspClient::Send(const char * msg, int size)
{
	if(!msg || size <= 0) {
		printf("Recv Argument Error\n");
		return TRANS_ERROR;
	}

	int SendResult = 0;
	if(!CheckSockWritable(RtspSockfd)) return TRANS_ERROR;
	SendResult = send(RtspSockfd, msg, size, 0);
	if(0 == SendResult) {
		printf("Socket disconnected\n");
		return TRANS_ERROR;
	}
	if(SendResult < 0 && 
			errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN) {
		return TRANS_ERROR;
	}

	return TRANS_OK;
}

int RtspClient::Send(string msg)
{
	return Send(msg.c_str(), msg.length());
}

int RtspClient::Recv(char * msg, int size)
{
	if(!msg || size <= 0) {
		printf("Recv Argument Error\n");
		return TRANS_ERROR;
	}

	int RecvResult = 0;
	if(!CheckSockReadable(RtspSockfd)) return TRANS_ERROR;
	RecvResult = recv(RtspSockfd, msg, size, 0);
	if(0 == RecvResult) {
		printf("Socket disconnected\n");
		return TRANS_ERROR;
	}
	if(RecvResult < 0 && 
			errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN) {
		return TRANS_ERROR;
	}

	return TRANS_OK;
}

int RtspClient::Recv(string * msg)
{
	if(!msg) {
		printf("Invalid Argument\n");
		return TRANS_ERROR;
	}

	char * Buf = (char *)calloc(RECV_BUF_SIZE, sizeof(char));
	int RecvResult = 0;

	msg->assign("");
	RecvResult = Recv(Buf, RECV_BUF_SIZE);
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

bool RtspClient::IsResponseOK(string response)
{
	// example: 
	// "RTSP/1.0 200 OK"
	string Pattern("RTSP/[0-9]+\\.[0-9]+ +200");
	list<string> Groups;
	bool IgnoreCase = true;
	bool Result = false;

	if(Regex.Regex(response.c_str(), Pattern.c_str(), &Groups, IgnoreCase)) {
		Result = true;
	}
	return Result;
}

