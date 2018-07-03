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
#include "myRtpTcpSession.h"
#include "MediaSession.h"
#include "myTCPTransmitter.h"

#define RTP_OK 		1
#define RTP_ERROR 	0

#define USLEEP_UNIT 	10000

using namespace jrtplib;

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

MyRTPTCPSession::MyRTPTCPSession():
	MyRTPSession()
{
    TunnellingSock = 0;
    pthread_mutex_init(&SocketMutex, NULL);
    TrylockTimes = 0;
}

MyRTPTCPSession::~MyRTPTCPSession()
{
    pthread_mutex_destroy(&SocketMutex);
}

int MyRTPTCPSession::MyRTP_SetUp(MediaSession * media_session, SocketType tunnelling_sock)
{
	if(!media_session) {
		fprintf(stderr, "%s: Invalid media session\n", __func__);
		return RTP_ERROR;
	}
	if(0 == media_session->TimeRate) {
		fprintf(stderr, "%s: Invalid MediaSession::TimeRate\n", __func__);
		return RTP_ERROR;
	}

	int status;

	// Now, we'll create a RTP session, set the destination
	// and poll for incoming data.

	RTPSessionParams sessparams;
	RTPTCPTransmissionParams transparams;

	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be just use 8000 samples per second.
	sessparams.SetOwnTimestampUnit(1.0/media_session->TimeRate);         

	sessparams.SetAcceptOwnPackets(true);

	sessparams.SetProbationType(RTPSources::NoProbation);
	// TODO: use a valueable instead of "65535"
	// sessparams.SetMaximumPacketSize(65535);

	//         bool threadsafe = false;
	// #ifdef RTP_SUPPORT_THREAD
	//         threadsafe = true;
	// #endif // RTP_SUPPORT_THREAD
	//         transparams.Init(threadsafe);
	// TODO: use a valueable instead of "65535"
	// transparams.Create(65535, 0);

	// status = Create(sessparams,&transparams, RTPTransmitter::TCPProto);  
	status = MyTcpCreate(sessparams,&transparams);  
    TunnellingSock = tunnelling_sock;
	AddDestination(RTPTCPAddress(tunnelling_sock));
	return IsError(status);
}

void MyRTPTCPSession::MyRTP_Teardown(MediaSession * media_session, struct timeval * tval)
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

uint8_t * MyRTPTCPSession::GetMyRTPData(uint8_t * data_buf, size_t * size, unsigned long timeout_ms)
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
        if(TryLockSocket()) {
            int status = Poll();
            // printf("DEBUG: end poll: %d\n", status);
            if(!IsError(status)) return NULL;
            UnlockSocket();

        }
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
		// printf("data length: %lu\t", PacketSize);
		// printf("%02x %02x %02x %02x\n", Packet[0], Packet[1], Packet[2], Packet[3]);

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

uint8_t * MyRTPTCPSession::GetMyRTPPacket(uint8_t * packet_buf, size_t * size, unsigned long timeout_ms)
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

void MyRTPTCPSession::LockSocket()
{
    pthread_mutex_lock(&SocketMutex);
}

void MyRTPTCPSession::UnlockSocket()
{
    pthread_mutex_unlock(&SocketMutex);
}

bool MyRTPTCPSession::TryLockSocket()
{
    if(0 != pthread_mutex_trylock(&SocketMutex)) {
        TrylockTimes++;
        if(TrylockTimes > 10) {
            printf("WARNING: There are some RTSP packages unreceived\n");
        }
        return false;
    }
    if(TrylockTimes > 0) {
        TrylockTimes--;
    }
    return true;
}


int MyRTPTCPSession::MyTcpCreate(const RTPSessionParams &sessparams,const RTPTransmissionParams *transparams)
{
	int status;
	
	if (created)
		return ERR_RTP_SESSION_ALREADYCREATED;

	usingpollthread = sessparams.IsUsingPollThread();
	needthreadsafety = sessparams.NeedThreadSafety();
	if (usingpollthread && !needthreadsafety)
		return ERR_RTP_SESSION_THREADSAFETYCONFLICT;

	useSR_BYEifpossible = sessparams.GetSenderReportForBYE();
	sentpackets = false;
	
	// Check max packet size
	
	if ((maxpacksize = sessparams.GetMaximumPacketSize()) < RTP_MINPACKETSIZE)
		return ERR_RTP_SESSION_MAXPACKETSIZETOOSMALL;
		
	// Initialize the transmission component
	
	rtptrans = 0;
    rtptrans = RTPNew(GetMemoryManager(),RTPMEM_TYPE_CLASS_RTPTRANSMITTER) MyTCPTransmitter(GetMemoryManager());
	
	if (rtptrans == 0)
		return ERR_RTP_OUTOFMEM;
	if ((status = rtptrans->Init(needthreadsafety)) < 0)
	{
		RTPDelete(rtptrans,GetMemoryManager());
		return status;
	}
	if ((status = rtptrans->Create(maxpacksize,transparams)) < 0)
	{
		RTPDelete(rtptrans,GetMemoryManager());
		return status;
	}

	deletetransmitter = true;
	return InternalCreate(sessparams);
}

void MyRTPTCPSession::OnNewSource(RTPSourceData *dat)
{
	// if (dat->IsOwnSSRC())
	// 	return;

	// uint32_t ip;
	// uint16_t port;

	// if (dat->GetRTPDataAddress() != 0)
	// {
	// 	const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
	// 	ip = addr->GetIP();
	// 	port = addr->GetPort();
	// }
	// else if (dat->GetRTCPDataAddress() != 0)
	// {
	// 	const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
	// 	ip = addr->GetIP();
	// 	port = addr->GetPort()-1;
	// }
	// else
	// 	return;

	// RTPIPv4Address dest(ip,port);
	// AddDestination(dest);

	// struct in_addr inaddr;
	// inaddr.s_addr = htonl(ip);
	std::cout << "Adding destination" << std::endl;
}

void MyRTPTCPSession::OnBYEPacket(RTPSourceData *dat)
{
	// if (dat->IsOwnSSRC())
	// 	return;

	// uint32_t ip;
	// uint16_t port;

	// if (dat->GetRTPDataAddress() != 0)
	// {
	// 	const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
	// 	ip = addr->GetIP();
	// 	port = addr->GetPort();
	// }
	// else if (dat->GetRTCPDataAddress() != 0)
	// {
	// 	const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
	// 	ip = addr->GetIP();
	// 	port = addr->GetPort()-1;
	// }
	// else
	// 	return;

	// RTPIPv4Address dest(ip,port);
	// DeleteDestination(dest);

	// struct in_addr inaddr;
	// inaddr.s_addr = htonl(ip);
	std::cout << "Deleting destination" << std::endl;
	if(DestroiedClbk) {
		DestroiedClbk();
	} 
}

void MyRTPTCPSession::OnRemoveSource(RTPSourceData *dat)
{
	// if (dat->IsOwnSSRC())
	// 	return;
	// if (dat->ReceivedBYE())
	// 	return;

	// uint32_t ip;
	// uint16_t port;

	// if (dat->GetRTPDataAddress() != 0)
	// {
	// 	const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
	// 	ip = addr->GetIP();
	// 	port = addr->GetPort();
	// }
	// else if (dat->GetRTCPDataAddress() != 0)
	// {
	// 	const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
	// 	ip = addr->GetIP();
	// 	port = addr->GetPort()-1;
	// }
	// else
	// 	return;

	// RTPIPv4Address dest(ip,port);
	// DeleteDestination(dest);

	// struct in_addr inaddr;
	// inaddr.s_addr = htonl(ip);
	std::cout << "Deleting destination" << std::endl;
	if(DestroiedClbk) {
		DestroiedClbk();
	}
}

void MyRTPTCPSession::SetRecvRtspCmdClbk(void (*clbk)(char * cmd))
{
    MyTCPTransmitter * session = static_cast<MyTCPTransmitter*>(rtptrans);
    if(session != NULL) {
        session->SetRecvRtspCmdClbk(clbk);
    }
}
