#ifndef MY_RTP_SESSION_H
#define MY_RTP_SESSION_H

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rtpsourcedata.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace jrtplib;
class MediaSession;

int checkerror(int rtperr);

class MyRTPSession : public RTPSession
{
	public:
		int MyRTP_SetUp(MediaSession * media_session);

		/* Wait 1 second for TEARDOWN at default */
		void MyRTP_Teardown(MediaSession * media_session, struct timeval * tval = NULL);
		uint8_t * GetMyRTPData(uint8_t * data_buf, size_t * size);
		uint8_t * GetMyRTPPacket(uint8_t * packet_buf, size_t * size);

	protected:
		int IsError(int rtperr);
		void OnNewSource(RTPSourceData *dat);
		void OnBYEPacket(RTPSourceData *dat);
		void OnRemoveSource(RTPSourceData *dat);
};

#endif
