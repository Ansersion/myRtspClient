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

#include "MediaSession.h"
#include "myRtpTcpSession.h"
#include "myRtpUdpSession.h"
#include "myRtpSession.h"
#include "nalu_types.h"
#include <vector>

using std::vector;

#define MEDIA_SESSION_OK 		1
#define MEDIA_SESSION_ERROR 	0

/*
   Refer to RTP PayloadType list
   "0" means that the item is reserved
   */
int PT2TimeRateMap[] =
{
	8000, 
	0, 
	0, 
	8000,
	8000,
	8000,
	16000,
	8000,
	8000,
	8000,
	44100, 
	44100, 
	8000,
	8000,
	90000,
	8000,
	11025, 
	22050, 
	8000, 
	0, 
	0, 
	0, 
	0, 
	0
};

MediaSession::MediaSession():
	MediaType(""),
	Protocol(""),
	EncodeType(""),
	TimeRate(0),
	ControlURI(""),
	SessionID(""),
	// RTSPSockfd(-1),
	RTPPort(0),
	// RTPSockfd(-1),
	RTCPPort(0),
	// RTCPSockfd(-1),
	Packetization(PACKET_MODE_SINGAL_NAL),
	RTPInterface(NULL),
	NalUnitType(NULL)

{
	ChannelNum = 1;
}

MediaSession::~MediaSession()
{
	RTP_Teardown();

	if(RTPInterface) delete RTPInterface;
}

int MediaSession::RTP_SetUp(int tunnelling_sock)
{
	if(0 == TimeRate) return MEDIA_SESSION_ERROR;
	if(0 == RTPPort) return MEDIA_SESSION_ERROR;
	if(RTPInterface) {
		printf("RTP SetUp already\n");
		return MEDIA_SESSION_OK;
	}

	printf("MyRTP_SetUp TCP: %d\n", tunnelling_sock);
    if(tunnelling_sock > 0) {
		RTPInterface = new MyRTPTCPSession;
	    if(!RTPInterface->MyRTP_SetUp(this, tunnelling_sock)) return MEDIA_SESSION_ERROR;
    } else {
		RTPInterface = new MyRTPUDPSession;
	    if(!RTPInterface->MyRTP_SetUp(this)) return MEDIA_SESSION_ERROR;
    }

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

int MediaSession::MediaInfoCheck()
{
	// Check "PayloadType"
	if(PayloadType.size() == 0) {
		printf("WARNING: invalid PayloadType\n");
		return -1;
	}
	for(vector<int>::iterator it = PayloadType.begin(); it != PayloadType.end(); it++) {
		if(*it < 0) {
			printf("WARNING: invalid PayloadType\n");
			return -1;
		}
	}	
	if(TimeRate <= 0) {
		TimeRate = PT2TimeRateMap[*(PayloadType.begin())]; // FIXME: only use the first PayloadType
		printf("MediaInfoCheck: %d\n", TimeRate);
	}
	return 0;
}

void MediaSession::LockSocket()
{
    if(RTPInterface) {
        RTPInterface->LockSocket();
    }
}

void MediaSession::UnlockSocket()
{
    if(RTPInterface) {
        RTPInterface->UnlockSocket();
    }
}

int MediaSession::GetTunnellingSocket()
{
    if(RTPInterface) {
        return RTPInterface->GetTunnellingSocket();
    }
    return -1;
}


void MediaSession::SetRtpDestroiedClbk(void (*clbk)()) { 
	if(RTPInterface) {
		RTPInterface->SetDestroiedClbk(clbk);
	}
}
