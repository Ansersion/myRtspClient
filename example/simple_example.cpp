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

	/* Send PLAY command to play all of the 'audio' and 
	 * 'video' sessions.*/
	Client.DoPLAY();

	/* Receive 1000 RTP 'video' packets
	 * note(FIXME): 
	 * if there are several 'video' session 
	 * refered in SDP, only receive packets of the first 
	 * 'video' session, the same as 'audio'.*/
	int packet_num = 0;
	while(++packet_num < 1000) {
		uint8_t buf[4192];
		size_t size = 0;
		Client.GetMediaPacket("video", buf, &size);
		printf("recv %lu\n", size);
	}

	printf("start TEARDOWN\n");
	/* Send TEARDOWN command to teardown all of the sessions */
	Client.DoTEARDOWN();

	return 0;
}
