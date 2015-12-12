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
#define SEARCH_PORT_RTP_FROM 	10000 // must be even number

enum SessionType {
    VIDEO_SESSION = 0, 
    AUDIO_SESSION
};

enum ErrorType {
    RTSP_NO_ERROR = 0,
    RTSP_INVALID_URI,
	RTSP_SEND_ERROR, 
	RTSP_RECV_ERROR,
	RTSP_INVALID_MEDIA_SESSION,
	RTSP_RESPONSE_BLANK,
	RTSP_RESPONSE_200,
	RTSP_RESPONSE_400,
	RTSP_RESPONSE_401,
	RTSP_RESPONSE_404,
	RTSP_RESPONSE_500,
	RTSP_RESPONSE_501,
    RTSP_UNKNOWN_ERROR
};

class MediaSession {
	public:
		MediaSession();
		string MediaType;
		string EncodeType;
		string Protocol;
		unsigned int TimeRate;
		string ControlURI;
		string SessionID;
		int RTSPSockfd;
		uint16_t RTPPort;
		int RTPSockfd;
		uint16_t RTCPPort;
		int RTCPSockfd;
};

// typedef shared_ptr< multimap<string, string> > 	 	SDPInfoPtr;
// typedef shared_ptr< map<string, MediaSessionPtr> > 	MediaSessionMapPtr;

class RtspClient
{
	public:
		RtspClient();
		RtspClient(string uri);
		~RtspClient();
		ErrorType DoDESCRIBE(string uri = "");
		ErrorType DoOPTIONS(string uri = "");
		ErrorType DoPAUSE();
		ErrorType DoSETUP();
		ErrorType DoSETUP(MediaSession * media_session);
		ErrorType DoPLAY();
		ErrorType DoPLAY(MediaSession * media_session);
		ErrorType DoPLAY(string media_type);
		ErrorType DoTEARDOWN();
		int ParseSDP(string SDP = "");
		string ParseSessionID(string ResponseOfSETUP = "");
		const MediaSession * GetVideoSession();
		const MediaSession * GetAudioSession();

		string ParseError(ErrorType et);

		void SetURI(const string uri) { RtspURI.assign(uri); };
		string GetURI() const { return RtspURI; };
		void SetPort(const int port) { RtspPort = port; };
		string GetResponse() const { return RtspResponse; };
		multimap<string, string> GetSDPInfo() const { return *SDPInfo; };

		/* Tools Methods */
		int CreateTcpSockfd(string uri = "");
		/* "CreateUdpSockfd" is only for test. 
		 * We will use jrtplib instead later. */
		int CreateRTP_RTCPSockfd(MediaSession * media_session, uint16_t RTP_port = 0); 
		in_addr_t GetIP(string uri = "");
		uint16_t GetPort(string uri = "");
		bool IsResponseOK(string response);
		map<string, MediaSession> GetMediaSessions() const { return *MediaSessionMap; }

	protected:
		int CheckSockWritable(int sockfd, struct timeval * tval = NULL);
		int CheckSockReadable(int sockfd, struct timeval * tval = NULL);
		int SendRTSP(const char * msg, size_t size);
		int SendRTSP(string msg);
		int RecvRTSP(char * msg, size_t max_line);
		int RecvRTSP(string * msg);

		/* "RecvSDP" can only be used after DESCRIBE. */
		int RecvSDP(int sockfd, char * msg, size_t size);
		int RecvSDP(int sockfd, string * msg = NULL); 
		int Close(int sockfd);

	protected:
		string RtspURI;
		unsigned int RtspCSeq;
		int RtspSockfd;
		string RtspIP;
		uint16_t RtspPort;
		string RtspResponse;
		multimap<string, string> *SDPInfo;
		map<string, MediaSession> *MediaSessionMap;

		MyRegex Regex;
};

#endif
