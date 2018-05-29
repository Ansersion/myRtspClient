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

#ifndef MY_RTP_SESSION_H
#define MY_RTP_SESSION_H

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>


typedef void (*DESTROIED_RTP_CLBK) ();

using namespace jrtplib;
class MediaSession;

int checkerror(int rtperr);

class MyRTPSession : public RTPSession
{
	public:
		MyRTPSession();
		int MyRTP_SetUp(MediaSession * media_session);

		/* Wait 1 second for TEARDOWN at default */
		void MyRTP_Teardown(MediaSession * media_session, struct timeval * tval = NULL);
		uint8_t * GetMyRTPData(uint8_t * data_buf, size_t * size, unsigned long timeout_ms);
		uint8_t * GetMyRTPPacket(uint8_t * packet_buf, size_t * size, unsigned long timeout_ms);

		void SetDestroiedClbk(void (*clbk)()) {DestroiedClbk = clbk;}

	protected:
		int IsError(int rtperr);
		void OnNewSource(RTPSourceData *dat);
		void OnBYEPacket(RTPSourceData *dat);
		void OnRemoveSource(RTPSourceData *dat);
		// void OnPollThreadStep();
		// void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
        void OnPollThreadError(int);
        void OnPollThreadStep();
        void OnPollThreadStart(bool &);
        void OnPollThreadStop();

	private:
		void (*DestroiedClbk)();

};

#endif
