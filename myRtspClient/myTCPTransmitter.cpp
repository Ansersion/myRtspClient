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

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "myRegex.h"
#include "rtprawpacket.h"
#include "myTCPTransmitter.h"
#include "myRtpSession.h"
#include "utils.h"

#define RTPTCPTRANS_MAXPACKSIZE							65535

#ifdef RTP_SUPPORT_THREAD
	#define MAINMUTEX_LOCK 		{ if (m_threadsafe) m_mainMutex.Lock(); }
	#define MAINMUTEX_UNLOCK	{ if (m_threadsafe) m_mainMutex.Unlock(); }
	#define WAITMUTEX_LOCK		{ if (m_threadsafe) m_waitMutex.Lock(); }
	#define WAITMUTEX_UNLOCK	{ if (m_threadsafe) m_waitMutex.Unlock(); }
#else
	#define MAINMUTEX_LOCK
	#define MAINMUTEX_UNLOCK
	#define WAITMUTEX_LOCK
	#define WAITMUTEX_UNLOCK
#endif // RTP_SUPPORT_THREAD



int MyTCPTransmitter::PollSocket(SocketType sock, SocketData &sdata)
{
    // sdata with no use

#ifdef RTP_SOCKETTYPE_WINSOCK
	unsigned long len;
#else 
	size_t len;
#endif // RTP_SOCKETTYPE_WINSOCK
	bool dataavailable;
    // int status = 0;
    // int offset;
    bool complete = false;

    do {
        len = 0;
        // offset = 0;
        RTPIOCTL(sock, FIONREAD, &len);

        if (len <= 0) 
            dataavailable = false;
        else
            dataavailable = true;

        if(dataavailable) {
            switch(m_recvstate) {
                case RECV_LEN: 
                    {
                        const int httpTunnelHeaderLen = 4;
                        if(recv(sock, m_httpTunnelHeaderBuffer, 1, MSG_WAITALL|MSG_PEEK) < 0) {
                            return ERR_RTP_TCPTRANS_ERRORINRECV;
                        }
                        if(m_httpTunnelHeaderBuffer[0] != '$' && m_httpTunnelHeaderBuffer[0] != 'R') {
                            break;
                        } 
                        if(recv(sock, m_httpTunnelHeaderBuffer, 2, MSG_WAITALL|MSG_PEEK) < 0) {
                            return ERR_RTP_TCPTRANS_ERRORINRECV;
                        }
                        if(m_httpTunnelHeaderBuffer[1] != 0 && m_httpTunnelHeaderBuffer[1] != 1 && m_httpTunnelHeaderBuffer[1] != 'T') {
                            break;
                        } 
                        if(m_httpTunnelHeaderBuffer[0] == 'R') {
                            if(recv(sock, m_httpTunnelHeaderBuffer, httpTunnelHeaderLen, MSG_WAITALL|MSG_PEEK) < 0) {
                                return ERR_RTP_TCPTRANS_ERRORINRECV;
                            }
                            if(0 == strncmp((char *)m_httpTunnelHeaderBuffer, "RTSP", 4)) {
                                // printf("Got RTSP Command\n");
                                if(RecvRtspCmd!= NULL) {
									// printf("RecvRtspCmd != NULL\n");
                                    MyRegex Regex;
                                    int RecvResult = 0;
                                    int Index = 0;
                                    int bufsize = 8192;
                                    char * rtspbuf = new char[bufsize];
                                    memset(rtspbuf, 0, bufsize);
                                    while(bufsize > 0) {
                                        RecvResult = ReadLine(sock, rtspbuf + Index, bufsize);
                                        if(RecvResult < 0) {
                                            if(errno == EINTR) continue;
                                            else if(errno == EWOULDBLOCK || errno == EAGAIN) {
                                                delete[] rtspbuf;
                                                rtspbuf = NULL;
                                                break;
                                            } else {
                                                delete[] rtspbuf;
                                                rtspbuf = NULL;
                                                break;
                                            }
                                        } else if(RecvResult == 0) {
                                            break;
                                        }
                                        /*
                                         * Single with "\r\n" or "\n" is the termination tag of RTSP.
                                         * */
                                        if(RecvResult <= (int)strlen("\r\n") &&
                                                Regex.Regex(rtspbuf+Index, "^(\r\n|\n)$")) {
                                            break;
                                        }
                                        Index += RecvResult;
                                        bufsize -= RecvResult;
                                    }
                                    RecvRtspCmd(rtspbuf);
                                    if(rtspbuf != NULL) {
                                        delete[] rtspbuf;
                                        rtspbuf = NULL;
                                    }
                                }
                                RecvRtspCmd = NULL;
                                break;
                            } else {
                                if(recv(sock, m_httpTunnelHeaderBuffer, 2, MSG_WAITALL) < 0) {
                                    return ERR_RTP_TCPTRANS_ERRORINRECV;
                                }
                                break;
                            }
                        }
                        if(recv(sock, m_httpTunnelHeaderBuffer, httpTunnelHeaderLen, MSG_WAITALL) < 0) {
                            return ERR_RTP_TCPTRANS_ERRORINRECV;
                        }
                        m_isrtp = 0 == m_httpTunnelHeaderBuffer[1] ? true : false;
                        m_dataLength = (m_httpTunnelHeaderBuffer[2] << 8) + m_httpTunnelHeaderBuffer[3];
                        m_lengthBufferOffset = 0;
                        m_recvstate = RECV_DATA;
                        // printf("header: %d, %d\n", m_isrtp, m_dataLength);

                        break;
                    }
                case RECV_DATA:
                    m_pDataBuffer = RTPNew(GetMemoryManager(), RTPMEM_TYPE_BUFFER_RECEIVEDRTPPACKET) uint8_t[m_dataLength];
                    if(0 == m_pDataBuffer) {
                        return ERR_RTP_OUTOFMEM;
                    }
                    if(recv(sock, m_pDataBuffer, m_dataLength, MSG_WAITALL) < 0) {
                        return ERR_RTP_TCPTRANS_ERRORINRECV;
                    }
                    m_recvstate = RECV_LEN;
                    complete = true;
                    // printf("data: complete\n");
                    break;
                //case COMMIT_BYE:
                //    break;
                //case GOT_BYE:
                //    break;
                default:
                    m_recvstate = RECV_LEN;
                    break;
            }
        }
    }while(dataavailable && !complete);

    if(complete) {
        RTPTime curtime = RTPTime::CurrentTime();
        uint8_t *pBuf = m_pDataBuffer;
        m_pDataBuffer = 0;
        if(pBuf) {
            RTPTCPAddress *pAddr = RTPNew(GetMemoryManager(),RTPMEM_TYPE_CLASS_RTPADDRESS) RTPTCPAddress(sock);
            if(0 == pAddr) {
                return ERR_RTP_OUTOFMEM;
            }
            RTPRawPacket *pPack = RTPNew(GetMemoryManager(),RTPMEM_TYPE_CLASS_RTPRAWPACKET) RTPRawPacket(pBuf, m_dataLength, pAddr, curtime, m_isrtp, GetMemoryManager());
            if (pPack == 0)
            {
                RTPDelete(pAddr,GetMemoryManager());
                RTPDeleteByteArray(pBuf,GetMemoryManager());
                return ERR_RTP_OUTOFMEM;
            }
            m_rawpacketlist.push_back(pPack);	
        }
    }

    return 0;
}

int MyTCPTransmitter::SendRTPRTCPData(const void *data,size_t len)
{
	if (!m_init)
		return ERR_RTP_TCPTRANS_NOTINIT;

	MAINMUTEX_LOCK
	
	if (!m_created)
	{
		MAINMUTEX_UNLOCK
		return ERR_RTP_TCPTRANS_NOTCREATED;
	}
	if (len > RTPTCPTRANS_MAXPACKSIZE)
	{
		MAINMUTEX_UNLOCK
		return ERR_RTP_TCPTRANS_SPECIFIEDSIZETOOBIG;
	}
	
	std::map<SocketType, SocketData>::iterator it = m_destSockets.begin();
	std::map<SocketType, SocketData>::iterator end = m_destSockets.end();

	vector<SocketType> errSockets;
	int flags = 0;
#ifdef RTP_HAVE_MSG_NOSIGNAL
	flags = MSG_NOSIGNAL;
#endif // RTP_HAVE_MSG_NOSIGNAL

	while (it != end)
	{
		uint8_t lengthBytes[4] = { '$', 0x01, (uint8_t)((len >> 8)&0xff), (uint8_t)(len&0xff) };
		SocketType sock = it->first;

		if (send(sock,(const char *)lengthBytes,4,flags) < 0 ||
			send(sock,(const char *)data,len,flags) < 0)
			errSockets.push_back(sock);
		++it;
	}
	
	MAINMUTEX_UNLOCK

	if (errSockets.size() != 0)
	{
		for (size_t i = 0 ; i < errSockets.size() ; i++)
			OnSendError(errSockets[i]);
	}

	// Don't return an error code to avoid the poll thread exiting
	// due to one closed connection for example

	return 0;
}

