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

	int packet_num = 0;
	/* Receive 1000 RTP media packets */
	while(++packet_num < 1000) {
		uint8_t buf[4192];
		size_t size = 0;
		Client.GetMediaPacket("video", buf, &size);
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
