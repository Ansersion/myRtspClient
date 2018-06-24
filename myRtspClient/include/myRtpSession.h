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

#define RTP_OK      1
#define RTP_ERROR   0

#define USLEEP_UNIT     10000

typedef void (*DESTROIED_RTP_CLBK) ();

using namespace jrtplib;
class MediaSession;

int checkerror(int rtperr);

class MyRTPSession : public RTPSession
{
	public:
		MyRTPSession() {};
		virtual int MyRTP_SetUp(MediaSession * media_session, SocketType tunnelling_sock) {return 0;}
		virtual int MyRTP_SetUp(MediaSession * media_session) { return 0;}

		/* Wait 1 second for TEARDOWN at default */
		virtual void MyRTP_Teardown(MediaSession * media_session, struct timeval * tval = NULL){}
		virtual uint8_t * GetMyRTPData(uint8_t * data_buf, size_t * size, unsigned long timeout_ms) {return NULL;}
		virtual uint8_t * GetMyRTPPacket(uint8_t * packet_buf, size_t * size, unsigned long timeout_ms) {return NULL;}

		virtual void SetDestroiedClbk(void (*clbk)()) {DestroiedClbk = clbk;}

	protected:
		virtual int IsError(int rtperr)
		{
			if (rtperr < 0)
			{   
				std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
				return RTP_ERROR;
			}   
			return RTP_OK;
		}

		virtual void OnNewSource(RTPSourceData *dat) {}
		virtual void OnBYEPacket(RTPSourceData *dat) {}
		virtual void OnRemoveSource(RTPSourceData *dat) {}
		// void OnPollThreadStep();
		// void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
        virtual void OnPollThreadError(int) {}
        virtual void OnPollThreadStep() {}
        virtual void OnPollThreadStart(bool &) {}
        virtual void OnPollThreadStop() {}

	private:
		void (*DestroiedClbk)();

    // private:
    //     bool isHttpTunneling;

};

#endif
