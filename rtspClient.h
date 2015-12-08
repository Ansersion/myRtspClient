#ifndef RTSP_CLIENT_H
#define RTSP_CLIENT_H

#include <map>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myRegex.h"

using std::map;
using std::string;
using std::multimap;

#define PORT_RTSP 				554
#define VERSION_RTSP 			"1.0"
#define SELECT_TIMEOUT_SEC 		1
#define SELECT_TIMEOUT_USEC 	0

#define CHECK_OK 				1
#define CHECK_ERROR				0

#define TRANS_OK 				1
#define TRANS_ERROR 			0

#define RECV_BUF_SIZE 			8192

enum SessionType {
    VIDEO_SESSION = 0, 
    AUDIO_SESSION
};

enum ErrorType {
    RTSP_NO_ERROR = 0,
    RTSP_INVALID_URI,
	RTSP_SEND_ERROR, 
	RTSP_RECV_ERROR,
	RTSP_RESPONSE_BLANK,
	RTSP_RESPONSE_200,
	RTSP_RESPONSE_400,
	RTSP_RESPONSE_401,
	RTSP_RESPONSE_404,
	RTSP_RESPONSE_500,
	RTSP_RESPONSE_501,
    RTSP_UNKNOWN_ERROR
};

typedef struct MediaSession {
	string MediaType;
	string EncodeType;
	unsigned int TimeRate;
	string ControlURI;
	uint16_t RTPPort;
	uint16_t RTCPPort;
}MediaSession;

class RtspClient
{
	public:
		RtspClient();
		RtspClient(string uri);
		~RtspClient();
		ErrorType DoDESCRIBE(string uri = "");
		ErrorType DoOPTIONS(string uri = "");
		ErrorType DoPAUSE();
		ErrorType DoPLAY();
		ErrorType DoSETUP(MediaSession * media_session = NULL);
		ErrorType DoTEARDOWN();
		int ParseSDP(string uri = "");

		string ParseError(ErrorType et);

		void SetURI(const string uri) { RtspURI.assign(uri); };
		string GetURI() const { return RtspURI; };
		void SetPort(const int port) { RtspPort = port; };
		string GetResponse() const { return RtspResponse; };
		multimap<string, string> GetSDPInfo() const { return SDPInfo; };

		/* Tools Methods */
		int CreateTcpSockfd(string uri = "");
		/* "CreateUdpSockfd" is only for test. 
		 * We will use jrtplib instead later. */
		int CreateUdpSockfd(uint16_t RTPPort); 
		in_addr_t GetIP(string uri = "");
		uint16_t GetPort(string uri = "");
		bool IsResponseOK(string response);
		map<string, MediaSession> GetMediaSessions() const { return MediaSessionMap; }

	protected:
		int CheckSockWritable(int sockfd);
		int CheckSockReadable(int sockfd);
		int Send(const char * msg, int size);
		int Send(string msg);
		int Recv(char * msg, int size);
		int Recv(string * msg);
		int Close(int sockfd);

	protected:
		string RtspURI;
		unsigned int RtspCSeq;
		int RtspSockfd;
		string RtspIP;
		uint16_t RtspPort;
		string RtspResponse;
		multimap<string, string> SDPInfo;
		map<string , MediaSession> MediaSessionMap;

		MyRegex Regex;
};

#endif
