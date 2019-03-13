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

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "myRtpUdpSession.h"
#include "MediaSession.h"
#include "myTCPTransmitter.h"

using namespace jrtplib;

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

MyRTPUDPSession::MyRTPUDPSession():
	MyRTPSession(), 
	DestroiedClbk(NULL)
{

}

int MyRTPUDPSession::MyRTP_SetUp(MediaSession * media_session)
{
	if(!media_session) {
		fprintf(stderr, "%s: Invalid media session\n", __func__);
		return RTP_ERROR;
	}
	if(0 == media_session->TimeRate) {
		fprintf(stderr, "%s: Invalid MediaSession::TimeRate\n", __func__);
		return RTP_ERROR;
	}
	if(0 == media_session->RTPPort) {
		fprintf(stderr, "%s: Invalid MediaSession::RTPPort\n", __func__);
		return RTP_ERROR;
	}

	int status;

	// Now, we'll create a RTP session, set the destination
	// and poll for incoming data.


	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be just use 8000 samples per second.
	sessparams.SetOwnTimestampUnit(1.0/media_session->TimeRate);         

	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(media_session->RTPPort);
	transparams.SetRTPReceiveBuffer(100000);
	status = Create(sessparams,&transparams);  
	// printf("DEBUG: create udpsession\n");
	return IsError(status);
}

void MyRTPUDPSession::MyRTP_Teardown(MediaSession * media_session, struct timeval * tval)
{
	struct timeval Timeout;

	if(!tval) {
		Timeout.tv_sec = 1; 
		Timeout.tv_usec = 0; 
	} else {
		Timeout.tv_sec = tval->tv_sec;
		Timeout.tv_usec = tval->tv_usec;
	}

	media_session->RTPPort = 0;
	BYEDestroy(RTPTime(Timeout.tv_sec, Timeout.tv_usec), 0, 0);
}

void MyRTPUDPSession::OnNewSource(RTPSourceData *dat)
{
	if (dat->IsOwnSSRC())
		return;

	uint32_t ip;
	uint16_t port;

	if (dat->GetRTPDataAddress() != 0)
	{
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort();
	}
	else if (dat->GetRTCPDataAddress() != 0)
	{
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort()-1;
	}
	else
		return;

	RTPIPv4Address dest(ip,port);
	AddDestination(dest);

	struct in_addr inaddr;
	inaddr.s_addr = htonl(ip);
	std::cout << "Adding destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
}

void MyRTPUDPSession::OnBYEPacket(RTPSourceData *dat)
{
	if (dat->IsOwnSSRC())
		return;

	uint32_t ip;
	uint16_t port;

	if (dat->GetRTPDataAddress() != 0)
	{
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort();
	}
	else if (dat->GetRTCPDataAddress() != 0)
	{
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort()-1;
	}
	else
		return;

	RTPIPv4Address dest(ip,port);
	DeleteDestination(dest);

	struct in_addr inaddr;
	inaddr.s_addr = htonl(ip);
	std::cout << "Deleting destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
	if(DestroiedClbk) {
		DestroiedClbk();
	} 
}

void MyRTPUDPSession::OnRemoveSource(RTPSourceData *dat)
{
	if (dat->IsOwnSSRC())
		return;
	if (dat->ReceivedBYE())
		return;

	uint32_t ip;
	uint16_t port;

	if (dat->GetRTPDataAddress() != 0)
	{
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort();
	}
	else if (dat->GetRTCPDataAddress() != 0)
	{
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort()-1;
	}
	else
		return;

	RTPIPv4Address dest(ip,port);
	DeleteDestination(dest);

	struct in_addr inaddr;
	inaddr.s_addr = htonl(ip);
	std::cout << "Deleting destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
	if(DestroiedClbk) {
		DestroiedClbk();
	}
}

uint8_t * MyRTPUDPSession::GetMyRTPData(uint8_t * data_buf, size_t * size, unsigned long timeout_ms)
{
	if(!data_buf) {
		fprintf(stderr, "%s: Invalide argument('data_buf==NULL')", __func__);
		return NULL;
	}

	if(!size) {
		fprintf(stderr, "%s: Invalide argument('size==NULL')", __func__);
		return NULL;
	}

	unsigned long UsleepTimes = (timeout_ms + USLEEP_UNIT - 1) / USLEEP_UNIT; // floor the 'timeout_ms / USLEEP_UNIT'
    *size = 0;

	do {
#ifndef RTP_SUPPORT_THREAD
		int status = Poll();
		// printf("DEBUG: end poll: %d\n", status);
		if(!IsError(status)) return NULL;
#endif 

		BeginDataAccess();

		// check incoming packets
		if (!GotoFirstSourceWithData()) {
			EndDataAccess();
			usleep(USLEEP_UNIT);
			UsleepTimes--;
            if(UsleepTimes <= 0) {
                break;
            }
			continue;
			// return NULL;
		}
		RTPPacket *pack;

		if(!(pack = GetNextPacket()))
		{
			EndDataAccess();
			usleep(USLEEP_UNIT);
			UsleepTimes--;
            if(UsleepTimes <= 0) {
                break;
            }
			continue;
			// return NULL;
		}

		size_t PacketSize = 0;
		uint8_t * Packet = NULL;
		Packet = pack->GetPayloadData();
		PacketSize = pack->GetPayloadLength();
		// printf("DEBUG: get packet: %d\n", PacketSize);
		//for(int i = 0; i < PacketSize; i++) {
		//	printf("%x ", Packet[i]);
		//}
		//printf("\n");
		// printf("data length: %lu\n", PacketSize);

		*size = PacketSize;
		memcpy(data_buf, Packet, PacketSize);

		// we don't longer need the packet, so
		// we'll delete it
		DeletePacket(pack);
		EndDataAccess();
		UsleepTimes = 0; // Got the data. So not need to sleep any more.
	} while(UsleepTimes > 0);

	return data_buf;
}

uint8_t * MyRTPUDPSession::GetMyRTPPacket(uint8_t * packet_buf, size_t * size, unsigned long timeout_ms)
{
	if(!packet_buf) {
		fprintf(stderr, "%s: Invalide argument('packet_buf==NULL')", __func__);
		return NULL;
	}

	if(!size) {
		fprintf(stderr, "%s: Invalide argument('size==NULL')", __func__);
		return NULL;
	}
    *size = 0;

	unsigned long UsleepTimes = (timeout_ms + USLEEP_UNIT - 1) / USLEEP_UNIT; // floor the 'timeout_ms / USLEEP_UNIT'

	do {
#ifndef RTP_SUPPORT_THREAD
		int status = Poll();
		if(!IsError(status)) return NULL;
#endif 

		BeginDataAccess();

		// check incoming packets
		if (!GotoFirstSourceWithData()) {
			EndDataAccess();
			usleep(USLEEP_UNIT);
			UsleepTimes--;
            if(UsleepTimes <= 0) {
                break;
            }
			continue;
			// return NULL;
		}
		RTPPacket *pack;

		if(!(pack = GetNextPacket()))
		{
			EndDataAccess();
			usleep(USLEEP_UNIT);
			UsleepTimes--;
            if(UsleepTimes <= 0) {
                break;
            }
			continue;
			// return NULL;
		}

		size_t PacketSize = 0;
		uint8_t * Packet = NULL;
		Packet = pack->GetPacketData();
		PacketSize = pack->GetPacketLength();
		// printf("packet length: %lu\n", PacketSize);

		*size = PacketSize;
		memcpy(packet_buf, Packet, PacketSize);

		// we don't longer need the packet, so
		// we'll delete it
		DeletePacket(pack);
		EndDataAccess();
		UsleepTimes = 0;
	} while(UsleepTimes > 0);

	return packet_buf;
}

void MyRTPUDPSession::OnPollThreadError(int)
{
}

void MyRTPUDPSession::OnPollThreadStep()
{
}

void MyRTPUDPSession::OnPollThreadStart(bool &)
{
    printf("RTP Poll start\n");
}

void MyRTPUDPSession::OnPollThreadStop()
{
    printf("RTP Poll stop\n");
}

