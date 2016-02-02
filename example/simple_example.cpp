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
	// string RtspUri("rtsp://192.168.81.145/ansersion");
	RtspClient Client;

	/* Set up rtsp server resource URI */
	Client.SetURI(RtspUri);
	
	/* Send DESCRIBE command to server */
	Client.DoDESCRIBE();

	/* Parse SDP message after sending DESCRIBE command */
	Client.ParseSDP();

	/* Send SETUP command to set up all 'audio' and 'video' 
	 * sessions which SDP refers. */
	Client.DoSETUP();

	printf("start PLAY\n");
	printf("SDP: %s\n", Client.GetSDP().c_str());

	/* Send PLAY command to play only 'video' sessions.*/
	Client.DoPLAY("video");

	/* Receive 1000 RTP 'video' packets
	 * note(FIXME): 
	 * if there are several 'video' session 
	 * refered in SDP, only receive packets of the first 
	 * 'video' session, the same as 'audio'.*/
	int packet_num = 0;
	const size_t BufSize = 98304;
	uint8_t buf[BufSize];
	size_t size = 0;

	/* Write h264 video data to file "test_packet_recv.h264" 
	 * Then it could be played by ffplay */
	int fd = open("test_packet_recv.h264", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);

	while(++packet_num < 1000) {
		if(!Client.GetMediaData("video", buf, &size, BufSize)) continue;
		if(write(fd, buf, size) < 0) {
			perror("write");
		}
		printf("recv %u\n", size);
	}

	printf("start TEARDOWN\n");
	/* Send TEARDOWN command to teardown all of the sessions */
	Client.DoTEARDOWN();

	return 0;
}
