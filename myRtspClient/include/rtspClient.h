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

#ifndef RTSP_CLIENT_H
#define RTSP_CLIENT_H

#include <map>
#include <stdio.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myRegex.h"
#include "MediaSession.h"
#include "nalu_types.h"
#include "nalu_types_h265.h"

using std::map;
using std::string;
using std::multimap;

#define PORT_RTSP 				554
#define VERSION_RTSP 			"1.0"
#define VERSION_HTTP 			"1.1"
#define SELECT_TIMEOUT_SEC 		1
#define SELECT_TIMEOUT_USEC 	0

#define CHECK_OK 				1
#define CHECK_ERROR				0

#define TRANS_OK 				1
#define TRANS_ERROR 			0

#define RECV_BUF_SIZE 			8192
#define SEARCH_PORT_RTP_FROM 	10330 // '10330' is chosen at random(must be a even number)

typedef void (*DESTROIED_CLBK) ();

enum SessionType {
    VIDEO_SESSION = 0, 
    AUDIO_SESSION
};

enum ErrorType {
    RTSP_NO_ERROR = 0,
    RTSP_INVALID_URI,
	RTSP_SEND_ERROR, 
	RTSP_RECV_ERROR,
	RTSP_PARSE_SDP_ERROR,
	RTSP_INVALID_MEDIA_SESSION,
	RTSP_RESPONSE_BLANK,
	RTSP_RESPONSE_200,
	RTSP_RESPONSE_400,
	RTSP_RESPONSE_401,
	RTSP_RESPONSE_404,
	RTSP_RESPONSE_40X,
	RTSP_RESPONSE_500,
	RTSP_RESPONSE_501,
	RTSP_RESPONSE_50X,
    RTSP_UNKNOWN_ERROR
};

#define MAX_SPS_SIZE 	256
typedef struct SPS_t{
	unsigned char Sps[MAX_SPS_SIZE];
	size_t Size;
} SPS_t;

#define MAX_PPS_SIZE 	256
typedef struct PPS_t {
	unsigned char Pps[MAX_PPS_SIZE];
	size_t Size;
} PPS_t;

typedef struct Buffer_t {
	uint8_t * Buf;
	size_t Size;
} Buffer_t;

class RtspClient
{
    public:
        static const string HttpHeadUserAgent;
        static const string HttpHeadXSessionCookie;
        static const string HttpHeadAccept;
        static const string HttpHeadPrama;
        static const string HttpHeadCacheControl;
        static const string HttpHeadContentType;
        static const string HttpHeadContentLength;
        static const string HttpHeadExpires;

	public:
		RtspClient();
		RtspClient(string uri);
		~RtspClient();
		ErrorType DoOPTIONS(string uri = "");
		ErrorType DoDESCRIBE(string uri = "");

		/* To setup all of the media sessions in SDP */
		ErrorType DoSETUP();

		/* To setup the media sessions */
		ErrorType DoSETUP(MediaSession * media_session);

		/* Example: DoSETUP("video");
		 * To setup the first video session in SDP
		 * */
		ErrorType DoSETUP(string media_type);

		/* To play all of the media sessions in SDP */
		ErrorType DoPLAY();
		
		/* To play the media sessions 
		 * media_session: 
		 * 	the media session
		 * scale: 
		 *	playing speed, such as 1.5 means 1.5 times of the normal speed, default NULL=normal speed
		 * start_time: 
		 *	start playing point, such as 20.5 means starting to play at 20.5 seconds, default NULL=play from 0 second or the PAUSE point
		 * end_time: 
		 *	end playing point, such as 20.5 means ending play at 20.5 seconds, default NULL=play to the end
		 * */
		ErrorType DoPLAY(MediaSession * media_session, float * scale = NULL, float * start_time = NULL, float * end_time = NULL);

		/* Example: DoPLAY("video");
		 * To play the first video session in SDP
		 * media_type: 
		 * "audio"/"video"
		 * scale: 
		 *	playing speed, such as 1.5 means 1.5 times of the normal speed, default NULL=normal speed
		 * start_time: 
		 *	start playing point, such as 20.5 means starting to play at 20.5 seconds, default NULL=play from 0 second or the PAUSE point
		 * end_time: 
		 *	end playing point, such as 20.5 means ending play at 20.5 seconds, default NULL=play to the end
		 * 
		 * */
		ErrorType DoPLAY(string media_type, float * scale = NULL, float * start_time = NULL, float * end_time = NULL);

		/* To pause all of the media sessions in SDP */
		ErrorType DoPAUSE();

		/* To pause the media sessions */
		ErrorType DoPAUSE(MediaSession * media_session);

		/* Example: DoPAUSE("video");
		 * To pause the first video session in SDP
		 * */
		ErrorType DoPAUSE(string media_type);

		/* To get parameters all of the media sessions in SDP 
		* The most general use is to keep the RTSP session alive: 
		* Invoke this function periodly within TIMEOUT(see: GetSessionTimeout()) */
		ErrorType DoGET_PARAMETER();

		/* To get parameters of the media sessions */
		ErrorType DoGET_PARAMETER(MediaSession * media_session);

		/* Example: DoGET_PARAMETER("video");
		 * To get parameters of the first video session in SDP
		 * */
		ErrorType DoGET_PARAMETER(string media_type);

		/* To teardown all of the media sessions in SDP */
		ErrorType DoTEARDOWN();

		/* To teardown the media sessions */
		ErrorType DoTEARDOWN(MediaSession * media_session);

		/* Example: DoTEARDOWN("video");
		 * To teardown the first video session in SDP
		 * */
		ErrorType DoTEARDOWN(string media_type);

        ErrorType DoRtspOverHttpGet();
        ErrorType DoRtspOverHttpPost();


		int GetTimeRate(string media_type);
		int GetChannelNum(string media_type);

		int ParseSDP(string SDP = "");
		string ParseSessionID(string ResponseOfSETUP = "");
		int ParseTimeout(string ResponseOfSETUP = "");
		const MediaSession * GetVideoSession();
		const MediaSession * GetAudioSession();

		string ParseError(ErrorType et);

		void SetURI(const string uri) { RtspURI.assign(uri); };
		string GetURI() const { return RtspURI; };
		void SetPort(const int port) { RtspPort = port; };
		string GetResponse() const { return RtspResponse; };
		string GetSDP() const { return SDPStr; }
		// multimap<string, string> GetSDPInfo() const { return *SDPInfo; };

		/* Tools Methods */
		int CreateTcpSockfd(string uri = "");
        int CreateTcpSockfd(uint16_t rtsp_over_http_data_port);

		/* "CreateUdpSockfd" is only for test. 
		 * We will use jrtplib instead later. */
		int SetAvailableRTPPort(MediaSession * media_session, uint16_t RTP_port = 0); 
		in_addr_t GetIP(string uri = "");
		uint16_t GetPort(string uri = "");
        string GetResource(string uri = "");

		// "IsResponse_200_OK" is really a ineffective method, should be modified in future.
		bool IsResponse_200_OK(ErrorType * err = NULL, string * response = NULL);
		map<string, MediaSession> GetMediaSessions() const { return *MediaSessionMap; }

	public:
		uint8_t * GetMediaData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size);
		uint8_t * GetMediaData(string media_type, uint8_t * buf, size_t * size, size_t max_size);

		uint8_t * GetMediaPacket(MediaSession * media_session, uint8_t * buf, size_t * size);
		uint8_t * GetMediaPacket(string media_type, uint8_t * buf, size_t * size);

		uint8_t * GetVPSNalu(uint8_t * buf, size_t * size);
		uint8_t * GetSPSNalu(uint8_t * buf, size_t * size);
		uint8_t * GetPPSNalu(uint8_t * buf, size_t * size);

		uint32_t CheckAuth(int sockfd, string cmd, string uri);
		string MakeMd5DigestResp(string realm, string cmd, string uri, string nonce, string username = "", string password = "");
		void SetUsername(string username) {Username.assign(username);}
		void SetPassword(string password) {Password.assign(password);}

	public:
		void SetDestroiedClbk(MediaSession * media_session, DESTROIED_CLBK clbk);
		void SetDestroiedClbk(string media_type, DESTROIED_CLBK clbk);
		void SetAudioByeFromServerClbk(DESTROIED_CLBK clbk);
		void SetVideoByeFromServerClbk(DESTROIED_CLBK clbk);

		int GetSessionTimeout(string media_type);
		int GetSessionTimeout(MediaSession * media_session);

	protected:

		uint8_t * GetVideoData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size, bool get_vps_sps_pps_periodly = true);
		uint8_t * GetAudioData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size);

		int CheckSockWritable(int sockfd, struct timeval * tval = NULL);
		int CheckSockReadable(int sockfd, struct timeval * tval = NULL);
		// int SendRTSP(int fd, const char * msg, size_t size, bool http_tunnel);
		ErrorType SendRTSP(int fd, const char * msg, size_t size);
		ErrorType SendRTSP(int fd, string msg);
        ErrorType SendRTSP(int fd, uint16_t http_tunnel_port, string msg);
		ErrorType RecvRTSP(int fd, char * msg, size_t max_line);
		// int RecvRTSP(int fd, char * msg, size_t max_line, bool http_tunnel);
		ErrorType RecvRTSP(int fd, string * msg);
		ErrorType RecvRTSP(int fd, uint16_t http_tunnel_port, string * msg);

		/* "RecvSDP" can only be used after DESCRIBE. */
		int RecvSDP(int sockfd, char * msg, size_t size);
		// int RecvSDP(int sockfd, char * msg, size_t size, bool http_tunnel);
		int RecvSDP(int sockfd, string * msg = NULL); 
		int Close(int sockfd);

    public:
        /* Especially for H264/H265 */
		void SetObtainVpsSpsPpsPeriodly(const bool enable) { ObtainVpsSpsPpsPeriodly = enable; };
		bool GetObtainVpsSpsPpsPeriodly() const { return ObtainVpsSpsPpsPeriodly; };
        void UpdateXSessionCookie();
        void SetHttpTunnelPort(uint16_t port) { RtspOverHttpDataPort = port; };


	protected:
		string RtspURI;
		unsigned int RtspCSeq;
		int RtspSockfd;
		string RtspIP;
		uint16_t RtspPort;
		string RtspResponse;
		// multimap<string, string> *SDPInfo;
		string SDPStr;
		map<string, MediaSession> *MediaSessionMap;

		string VPS;
		string SPS;
		string PPS;
		bool CmdPLAYSent;

		// uint8_t AudioBuffer[BUFSIZ];
		// uint8_t VideoBuffer[BUFSIZ];
		Buffer_t AudioBuffer;
		Buffer_t VideoBuffer;

		// NALUTypeBase * NALUType;
		size_t GetVideoDataCount;

		// Authentication
		string Username;
		string Password;
		string Realm;
		string Nonce;
		DESTROIED_CLBK ByeFromServerAudioClbk;
		DESTROIED_CLBK ByeFromServerVideoClbk;
		int Timeout;

        /* Especially for H264/H265 */
        bool ObtainVpsSpsPpsPeriodly;

		uint16_t RtspOverHttpDataPort;
		int RtspOverHttpDataSockfd;

    protected:
        string HttpHeadUserAgentContent;
        string HttpHeadXSessionCookieContent;
        string HttpHeadAcceptContent;
        string HttpHeadPramaContent;
        string HttpHeadCacheControlContent;
        string HttpHeadContentTypeContent;
        string HttpHeadContentLengthContent;
        string HttpHeadExpiresContent;
};

#endif
