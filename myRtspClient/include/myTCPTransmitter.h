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

#ifndef MY_TCP_TRANSMITTER
#define MY_TCP_TRANSMITTER

#include "rtpconfig.h"
#include "rtpsocketutil.h"
#include "rtpsocketutilinternal.h"
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"
#include "rtptcpaddress.h"
#include "rtptcptransmitter.h"
#include "rtppacket.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;
using namespace jrtplib;

class MyTCPTransmitter : public RTPTCPTransmitter
{
public:
	MyTCPTransmitter(const string &name) : RTPTCPTransmitter(0), m_name(name) { }

	void OnSendError(SocketType sock)
	{
		cout << m_name << ": Error sending over socket " << sock << ", removing destination" << endl;
		DeleteDestination(RTPTCPAddress(sock));
	}
	
	void OnReceiveError(SocketType sock)
	{
		cout << m_name << ": Error receiving from socket " << sock << ", removing destination" << endl;
		DeleteDestination(RTPTCPAddress(sock));
	}
private:
	string m_name;
};

#endif
