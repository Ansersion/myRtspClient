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
	RTPSockfd(-1),
	RTCPPort(0),
	RTCPSockfd(-1),
	RTPInterface(NULL)
{
}

MediaSession::~MediaSession()
{
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
	if(!RTPInterface->RTP_SetUp(this)) return MEDIA_SESSION_ERROR;

	return MEDIA_SESSION_OK;
}

int MediaSession::RTP_TEARDOWN()
{
	return MEDIA_SESSION_OK;
}
