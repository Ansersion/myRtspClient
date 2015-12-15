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

		/* Wait 1 second for TEARDOWN at default */
	 	int RTP_Teardown(struct timeval * tval = NULL);

		uint8_t * GetMediaData(uint8_t * buf, size_t * size);

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
