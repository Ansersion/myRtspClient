//   Copyright 2018 Ansersion
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

#ifndef SDP_DATA_H
#define SDP_DATA_H

#include <iostream>
#include <map>
#include <string>
#include <stdint.h>

using std::map;
using std::string;

enum SDP_ATTR_ENUM {
    MEDIA_TYPE_NAME, // "video", "audio"
    CODEC_TYPE, // "H264", "MPA"...
    TIME_RATE, // "90000", "8000"...
    CHANNEL_NUM, // "1", "2"...
    PACK_MODE, // for h264/h265
    SPS, // for h264/h265
    PPS, // for h264/h265
    VPS, // for h265
    CONTROL_URI, // "rtsp://127.0.0.1:554/ansersion/trackID=0"...

};

// o=<username> <session id> <version> <network type> <address type> <address>
typedef struct SDPOriginStruct
{
    string userName;
    long sessionId;
    long version;
    string networkType;
    string addressType;
    string address;

    // SDPOriginStruct {
    //     userName = "";
    //     sessionId = 0;
    //     version = 0;
    //     networkType = "";
    //     addressType = "";
    //     address = "";
    // };
} OriginStruct;

typedef struct SDPConnectionData {
    string networkType;
    string addressType;
    string address;

    // SDPConnectionData {
    //     networkType = "";
    //     addressType = "";
    //     address = "";
    // };
} SDPConnectionData;

typedef struct SDPSessionTime {
    long startTime;
    long stopTime;

    // SDPSessionTime {
    //     startTime = 0;
    //     stopTime = 0;
    // };
} SDPSessionTime;

typedef struct SDPMediaInfo {
    string mediaType;
    string ports;
    string transProt;
    map<int, map<SDP_ATTR_ENUM, string> > fmtMap;
    string controlURI;

    // SDPMediaInfo {
    //     mediaType = "";
    //     ports = "";
    //     transProt = "";
    //     fmtMap.clear();
    //     controlURI = "";
    // };

} SDPMediaInfo;

class SDPData
{
	public:
        ~SDPData();
        void parse(string sdp);

	private:
        /* RFC2327.6 */
        int sdpVersion;
        string sessionName;
        SDPOriginStruct sdpOriginStruct;
        SDPConnectionData sdpConnectionData;;
        SDPSessionTime sdpSessionTime;
        map<string, SDPMediaInfo> mediaInfoMap;

};

#endif
