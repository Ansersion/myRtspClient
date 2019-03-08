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

bool ByeFromServerFlag = false;
void ByeFromServerClbk()
{
	cout << "Server send BYE" << endl;
	ByeFromServerFlag = true;
}

int main(int argc, char *argv[])
{
	if(argc != 2) {
		cout << "Usage: " << argv[0] << " <URL>" << endl;
		cout << "For example: " << endl;
		cout << argv[0] << " rtsp://127.0.0.1/ansersion" << endl;
		return 1;
	}
	// float scale = -1;
	// float start_time = 70.5;
	// float end_time = 50.5;
	cout << "Start play " << argv[1] << endl;
	cout << "Then put video data into test_packet_recv.h264" << endl;
	string RtspUri(argv[1]);

	RtspClient Client(RtspUri);

	/* Set rtsp access username */
	// Client.SetUsername("Ansersion");

	/* Set rtsp access password */
	// Client.SetPassword("AnsersionPassword");

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
	Client.SetVideoByeFromServerClbk(ByeFromServerClbk);
	printf("%s\n", Client.GetResponse().c_str());
	
	/* Check whether server return '200'(OK) */
	if(!Client.IsResponse_200_OK()) {
		printf("DoSETUP error\n");
		return 0;
	}

	printf("start PLAY\n");
	/* Send PLAY command to play only 'video' sessions.
	 * note(FIXME): 
	 * if there are several 'video' session 
	 * refered in SDP, only play the first 'video' 
	 * session, the same as 'audio'.*/
	if(Client.DoPLAY("video", NULL, NULL, NULL) != RTSP_NO_ERROR) {
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
	// int packet_num = 0;
	int try_times = 0;
	const size_t BufSize = 98304;
	uint8_t buf[BufSize];
	size_t size = 0;
    size_t write_size = 0;

	/* Write h264 video data to file "test_packet_recv.h264" 
	 * Then it could be played by ffplay */
	// int fd = open("test_packet_recv.h264", O_CREAT | O_RDWR, 0);
	int fd = open("test_packet_recv.h264", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);

    /* By default, myRtspClient will write SPS, PPS, VPS(H265 only) for H264/H265 video
       periodly when you invoke 'GetMediaData'. It could bring your video data stability.
       However, if you want a high performance, you could turn down this function and get 
       SPS, PPS, VPS by yourself just at the begining of the video data */
    /* For convenience, you could refer to "simple_example.cpp" */
    /* Get SPS, PPS, VPS manually start */
    Client.SetObtainVpsSpsPpsPeriodly(false);
    // if(!Client.GetVPSNalu(buf, &size)) {
	// 	if(write(fd, buf, size) < 0) {
	// 		perror("write");
	// 	}
    // } 
    // if(!Client.GetSPSNalu(buf, &size)) {
	// 	if(write(fd, buf, size) < 0) {
	// 		perror("write");
	// 	}
    // } 
    // if(!Client.GetPPSNalu(buf, &size)) {
	// 	if(write(fd, buf, size) < 0) {
	// 		perror("write");
	// 	}
    // } 
    /* Get SPS, PPS, VPS manually end */
    

	while(true) {
		if(!Client.GetMediaData("video", buf+write_size, &size, BufSize)) {
            if(ByeFromServerFlag) {
				printf("ByeFromServerFlag\n");
                break;
            }
            if(try_times > 5) {
				printf("try_times > 5\n");
				break;
			}
			try_times++;
			continue;
		}
        write_size += size;

        /* lower the 'write' times to improve performance */
        /* For convenience, you could refer to "simple_example.cpp" */
        if(write_size > 32768) {
            if(write(fd, buf, write_size) < 0) {
                perror("write");
            }
            write_size = 0;
        }

		try_times = 0;
		printf("recv %lu\n", size);
	}
    if(write_size > 0) {
        if(write(fd, buf, write_size) < 0) {
            perror("write");
        }
    }

	printf("start TEARDOWN\n");
    int err = Client.DoTEARDOWN();
	/* Send TEARDOWN command to teardown all of the sessions */
	if(err != RTSP_NO_ERROR && err != RTSP_INVALID_MEDIA_SESSION) {
		printf("DoTEARDOWN error: %d\n", err);
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
