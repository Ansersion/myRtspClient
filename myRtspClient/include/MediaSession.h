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

#ifndef MEDIA_SESSION_H
#define MEDIA_SESSION_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>

#define TIMEOUT_MICROSECONDS 	1000000 // wait for a packet at most 1 second

class MyRTPSession;
class StreamParameters;

class StreamParameters // ###2015-01-11### //
{
	public:
		/* For general media session */
		unsigned int PayloadNum;
		std::string EncodeType;
		unsigned int TimeRate;

		/* For H264 and H265*/
		int Packetization; 
		std::string SPS;
		std::string PPS;
};

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
		std::vector<uint16_t> Ports; // RTP and RTCP ports, -1 indicate none. ###2015-01-11### //
		std::string Protocol;

		std::vector<int> PayloadNum; // ###2015-01-11### //
		std::string EncodeType;
		unsigned int TimeRate;
		// std::map<unsigned int, StreamParameters> StreamParams;

		std::string ControlURI;
		std::string SessionID;
		// int RTSPSockfd;
		uint16_t RTPPort;
		// int RTPSockfd;
		uint16_t RTCPPort;
		// int RTCPSockfd;
		int Packetization;

	protected:
		MyRTPSession * RTPInterface;
};
 
#endif
