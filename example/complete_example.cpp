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

#include <iostream>
#include "rtspClient.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
	string RtspUri("rtsp://127.0.0.1/ansersion");
	RtspClient Client(RtspUri);

	/* Send OPTIONS command to server */
	if(Client.DoOPTIONS() != RTSP_NO_ERROR) {
		printf("DoOPTIONS error\n");
		return 0;
	}
	printf("%s\n", Client.GetResponse().c_str());
	/* Check whether server return '200'(OK) */
	if(!Client.IsResponse_200_OK()) {
		printf("DoOPTIONS error\n");
		return 0;
	}

	/* Send DESCRIBE command to server */
	if(Client.DoDESCRIBE() != RTSP_NO_ERROR) {
		printf("DoDESCRIBE error\n");
		return 0;
	}
	printf("%s\n", Client.GetResponse().c_str());
	/* Check whether server return '200'(OK) */
	if(!Client.IsResponse_200_OK()) {
		printf("DoDESCRIBE error\n");
		return 0;
	}

	/* Parse SDP message after sending DESCRIBE command */
	printf("%s\n", Client.GetSDP().c_str());
	if(Client.ParseSDP() != RTSP_NO_ERROR) {
		printf("ParseSDP error\n");
		return 0;
	}

	/* Send SETUP command to set up all 'audio' and 'video' 
	 * sessions which SDP refers. */
	if(Client.DoSETUP() != RTSP_NO_ERROR) {
		printf("DoSETUP error\n");
		return 0;
	}
	printf("%s\n", Client.GetResponse().c_str());
	
	/* Check whether server return '200'(OK) */
	if(!Client.IsResponse_200_OK()) {
		printf("DoSETUP error\n");
		return 0;
	}

	printf("start PLAY\n");
	/* Send PLAY command to play the 'video' session.
	 * note(FIXME): 
	 * if there are several 'video' session 
	 * refered in SDP, only play the first 'video' 
	 * session, the same as 'audio'.*/
	if(Client.DoPLAY("video") != RTSP_NO_ERROR) {
		printf("DoPLAY error\n");
		return 0;
	}
	
	printf("%s\n", Client.GetResponse().c_str());
	/* Check whether server return '200'(OK) */
	if(!Client.IsResponse_200_OK()) {
		printf("DoPLAY error\n");
		return 0;
	}

	/* Receive 1000 RTP 'video' packets
	 * note(FIXME): 
	 * if there are several 'video' session 
	 * refered in SDP, only receive packets of the first 
	 * 'video' session, the same as 'audio'.*/
	int packet_num = 0;
	uint8_t buf[65534];
	size_t size = 0;

	/* Write h264 video data to file "test_packet_recv.h264" 
	 * Then it could be played by ffplay */
	// int fd = open("test_packet_recv.h264", O_CREAT | O_RDWR, 0);
	int fd = open("test_packet_recv.h264", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
	if(Client.GetSPSNalu(buf, &size)) {
		if(write(fd, buf, size) < 0) {
			perror("write");
		}
	} else {
		printf("SPS error\n");
	}

	if(Client.GetPPSNalu(buf, &size)) {
		if(write(fd, buf, size) < 0) {
			perror("write");
		}
	} else {
		printf("PPS error\n");
	}

	while(++packet_num < 1000) {
		if(!Client.GetMediaData("video", buf, &size)) continue;
		if(write(fd, buf, size) < 0) {
			perror("write");
		}
		printf("recv %lu\n", size);
	}

	printf("start TEARDOWN\n");
	/* Send TEARDOWN command to teardown all of the sessions */
	if(Client.DoTEARDOWN() != RTSP_NO_ERROR) {
		printf("DoTEARDOWN error\n");
		return 0;
	}

	printf("%s\n", Client.GetResponse().c_str());
	/* Check whether server return '200'(OK) */
	if(!Client.IsResponse_200_OK()) {
		printf("DoTEARDOWN error\n");
		return 0;
	}
	return 0;
}
