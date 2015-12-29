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
#define SEARCH_PORT_RTP_FROM 	10330 // '10330' is chosen at random(must be a even number)

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

class RtspClient
{
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
		
		/* To play the media sessions */
		ErrorType DoPLAY(MediaSession * media_session);

		/* Example: DoPLAY("video");
		 * To play the first video session in SDP
		 * */
		ErrorType DoPLAY(string media_type);

		/* To pause all of the media sessions in SDP */
		ErrorType DoPAUSE();

		/* To pause the media sessions */
		ErrorType DoPAUSE(MediaSession * media_session);

		/* Example: DoPAUSE("video");
		 * To pause the first video session in SDP
		 * */
		ErrorType DoPAUSE(string media_type);

		/* To teardown all of the media sessions in SDP */
		ErrorType DoTEARDOWN();

		/* To teardown the media sessions */
		ErrorType DoTEARDOWN(MediaSession * media_session);

		/* Example: DoTEARDOWN("video");
		 * To teardown the first video session in SDP
		 * */
		ErrorType DoTEARDOWN(string media_type);

		int ParseSDP(string SDP = "");
		string ParseSessionID(string ResponseOfSETUP = "");
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

		/* "CreateUdpSockfd" is only for test. 
		 * We will use jrtplib instead later. */
		int SetAvailableRTPPort(MediaSession * media_session, uint16_t RTP_port = 0); 
		in_addr_t GetIP(string uri = "");
		uint16_t GetPort(string uri = "");

		// "IsResponse_200_OK" is really a ineffective method, should be modified in future.
		bool IsResponse_200_OK(ErrorType * err = NULL, string * response = NULL);
		map<string, MediaSession> GetMediaSessions() const { return *MediaSessionMap; }

		uint8_t * GetMediaData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size);
		uint8_t * GetMediaData(string media_type, uint8_t * buf, size_t * size, size_t max_size);

		uint8_t * GetMediaPacket(MediaSession * media_session, uint8_t * buf, size_t * size);
		uint8_t * GetMediaPacket(string media_type, uint8_t * buf, size_t * size);

		uint8_t * GetSPSNalu(uint8_t * buf, size_t * size);
		uint8_t * GetPPSNalu(uint8_t * buf, size_t * size);
	protected:

		uint8_t * GetVideoData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size);

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
		// multimap<string, string> *SDPInfo;
		string SDPStr;
		map<string, MediaSession> *MediaSessionMap;

		string SPS;
		string PPS;
		bool CmdPLAYSent;

		MyRegex Regex;
		uint8_t AudioBuffer[BUFSIZ];
		uint8_t VideoBuffer[BUFSIZ];

		// NALUTypeBase * NALUType;
};

#endif
