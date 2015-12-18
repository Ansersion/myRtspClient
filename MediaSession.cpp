#include "MediaSession.h"
#include "myRtpSession.h"

#define MEDIA_SESSION_OK 		1
#define MEDIA_SESSION_ERROR 	0

MediaSession::MediaSession():
	MediaType(""),
	EncodeType(""),
	Protocol(""),
	TimeRate(0),
	ControlURI(""),
	SessionID(""),
	// RTSPSockfd(-1),
	RTPPort(0),
	// RTPSockfd(-1),
	RTCPPort(0),
	// RTCPSockfd(-1),
	RTPInterface(NULL)
{
}

MediaSession::~MediaSession()
{
	RTP_Teardown();

	if(RTPInterface) delete RTPInterface;
}

int MediaSession::RTP_SetUp()
{
	if(0 == TimeRate) return MEDIA_SESSION_ERROR;
	if(0 == RTPPort) return MEDIA_SESSION_ERROR;
	if(RTPInterface) {
		printf("RTP SetUp already\n");
		return MEDIA_SESSION_OK;
	}

	RTPInterface = new MyRTPSession;
	if(!RTPInterface->MyRTP_SetUp(this)) return MEDIA_SESSION_ERROR;

	return MEDIA_SESSION_OK;
}

int MediaSession::RTP_Teardown(struct timeval * tval)
{
	if(!RTPInterface) {
		return MEDIA_SESSION_OK;
	}
	if(0 == RTPPort) return MEDIA_SESSION_ERROR;

	RTPPort = 0;
	// RTPSockfd = -1;
	RTCPPort = 0;
	// RTCPSockfd = -1;

	RTPInterface->MyRTP_Teardown(this, tval);
	delete RTPInterface;
	RTPInterface = NULL;
	return MEDIA_SESSION_OK;
}

uint8_t * MediaSession::GetMediaData(uint8_t * buf, size_t * size, unsigned long timeout) {
	if(!RTPInterface) return NULL;
	return  RTPInterface->GetMyRTPData(buf, size, timeout);
}

uint8_t * MediaSession::GetMediaPacket(uint8_t * buf, size_t * size, unsigned long timeout) {
	if(!RTPInterface) return NULL;
	return  RTPInterface->GetMyRTPPacket(buf, size, timeout);
}
