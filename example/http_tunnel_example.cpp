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

/************************************************************************************************************************************************
* a example to receive rtsp/rtp packets over http
* there are 2 tcp sockets(one sending, one receiving) for http-tunnelling communication.
* the receiving one will receiving both Rtsp response and media data. 
* after you DoSETUP, the media session will set up RTP which will take over the receiving socket.
*
* the 'getdata' thead to get media data and the 'sendrtspcmd' thread to send rtsp command, the rtsp response will be handled in RecvRtspCmdClbk.
* if you don't care about the response, it will be dropped if you don't set the callback(refer to http_tunnel_example_simple.cpp")
* 
* WARNING: you can receive more one media data within one RtspClient like 'recv_video_and_audio_example'
* That means you should do something like:
* RtspClient client_audio; (for audio receiving)
* RtspClient client_video; (for video receiving)
* NOT:
* RtspClient client; (for audio and video receiving)
************************************************************************************************************************************************/


#include <iostream>
#include "rtspClient.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using std::cout;
using std::endl;

void * getdata(void * args);
void * sendrtspcmd(void * args);


bool ByeFromServerFlag = false;
void ByeFromServerClbk()
{
	cout << "Server send BYE" << endl;
	ByeFromServerFlag = true;
}

// callback function to handle rtsp response
void RecvRtspCmdClbk(char * cmd) {
	printf("RecvRtspCmdClbk: %s", cmd);
}

int main(int argc, char *argv[])
{
	pthread_t getdata_thread;
	pthread_t sendrtspcmd_thread;
	if(argc != 2) {
		cout << "Usage: " << argv[0] << " <URL>" << endl;
		cout << "For example: " << endl;
		cout << argv[0] << " rtsp://127.0.0.1/ansersion" << endl;
		return 1;
	}
	cout << "Start play " << argv[1] << endl;
	cout << "Then put video data into test_packet_recv.h264" << endl;
	string RtspUri(argv[1]);

	// string RtspUri("rtsp://192.168.81.145/ansersion");
	RtspClient Client;

	/* Set up rtsp server resource URI */
	Client.SetURI(RtspUri);
    Client.SetHttpTunnelPort(8000);
    Client.SetPort(8000);
    if(RTSP_NO_ERROR == Client.DoRtspOverHttpGet()) {
        cout << "DoGet OK" << endl;
    }
    cout << Client.GetResource();
    if(RTSP_NO_ERROR == Client.DoRtspOverHttpPost()) {
        cout << "DoPost OK" << endl;
    }

    // /* Set rtsp access username */
    Client.SetUsername("Ansersion");

    // /* Set rtsp access password */
    Client.SetPassword("AnsersionPassword");

    // /* Send DESCRIBE command to server */
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
	Client.SetVideoByeFromServerClbk(ByeFromServerClbk);

	if(!Client.IsResponse_200_OK()) {
		printf("DoSETUP error\n");
		return 0;
	}


	pthread_create(&sendrtspcmd_thread, NULL, sendrtspcmd, (void*)(&Client));
	pthread_create(&getdata_thread, NULL, getdata, (void*)(&Client));
	pthread_join(sendrtspcmd_thread, NULL);
	pthread_join(getdata_thread, NULL);
	
    return 0;
}

void * getdata(void * args)
{
	/* Receive 1000 RTP 'video' packets
	 * note(FIXME): 
	 * if there are several 'video' session 
	 * refered in SDP, only receive packets of the first 
	 * 'video' session, the same as 'audio'.*/
	// int packet_num = 0;
	const size_t BufSize = 65534;
	uint8_t buf[BufSize];
	size_t size = 0;
	int try_times = 0;

	RtspClient * Client = (RtspClient*)args;

	/* Write h264 video data to file "test_packet_recv.h264" 
	 * Then it could be played by ffplay */
	int fd = open("test_packet_recv.h264", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);

	while(true) {
		if(!Client->GetMediaData("video", buf, &size, BufSize)) {
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
		if(write(fd, buf, size) < 0) {
			perror("write");
		}
		printf("recv video data: %lu bytes\n", size);
	}
	return NULL;
}

void * sendrtspcmd(void * args)
{

	bool no_response = true;
	RtspClient * Client = (RtspClient*)args;

	Client->SetRtspCmdClbk("video", RecvRtspCmdClbk);
	if(Client->DoPLAY("video", NULL, NULL, NULL, no_response) != RTSP_NO_ERROR) {
		printf("DoPLAY error\n");
		return 0;
	}
	sleep(5);
	printf("start TEARDOWN\n");
	/* Send TEARDOWN command to teardown all of the sessions */
	Client->SetRtspCmdClbk("video", RecvRtspCmdClbk);
	Client->DoTEARDOWN("video", no_response);
	return NULL;

}
