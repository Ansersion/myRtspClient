#ifndef MEDIA_SESSION_H
#define MEDIA_SESSION_H

#include <iostream>
#include <string>
#include <stdint.h>

#define TIMEOUT_MICROSECONDS 	1000000 // wait for a packet at most 1 second

class MyRTPSession;

class MediaSession
{
	public:
	 	MediaSession();
		~MediaSession();
	 	int RTP_SetUp();

		/* Wait 1 second for TEARDOWN at default */
		int RTP_Teardown(struct timeval * tval = NULL);

		/* Function Name: GetMediaData;
		 * Description: Get RTP payload;
		 * note:
		 * <timeout_ms> in unit of microsecond.
		 * Why we set 'timeout' here is to avoid continuously occupying CPU.
		 * */
		uint8_t * GetMediaData(uint8_t * buf, size_t * size, unsigned long timeout_ms = TIMEOUT_MICROSECONDS);

		/* Function Name: GetMediaPacket;
		 * Description: Get RTP Packet;
		 * note:
		 * <timeout_ms> in unit of microsecond.
		 * Why we set 'timeout' here is to avoid continuously occupying CPU.
		 * */
		uint8_t * GetMediaPacket(uint8_t * buf, size_t * size, unsigned long timeout_ms = TIMEOUT_MICROSECONDS);

	public:
		std::string MediaType;
		std::string EncodeType;
		std::string Protocol;
		unsigned int TimeRate;
		std::string ControlURI;
		std::string SessionID;
		// int RTSPSockfd;
		uint16_t RTPPort;
		// int RTPSockfd;
		uint16_t RTCPPort;
		// int RTCPSockfd;

	protected:
		MyRTPSession * RTPInterface;
};
 
#endif
