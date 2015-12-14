#ifndef MEDIA_SESSION_H
#define MEDIA_SESSION_H

#include <iostream>
#include <string>
#include <stdint.h>

class MyRTPSession;

class MediaSession
{
	public:
	 	MediaSession();
		~MediaSession();
	 	int RTP_SetUp();
	 	int RTP_TEARDOWN();

	public:
		std::string MediaType;
		std::string EncodeType;
		std::string Protocol;
		unsigned int TimeRate;
		std::string ControlURI;
		std::string SessionID;
		// int RTSPSockfd;
		uint16_t RTPPort;
		int RTPSockfd;
		uint16_t RTCPPort;
		int RTCPSockfd;

	protected:
		MyRTPSession * RTPInterface;
};
 
#endif
