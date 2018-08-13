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

#include "myRegex.h"
#include "sdp_data.h"
#include <sstream>
#include <stdlib.h>

using std::stringstream;
using std::cout;
using std::endl;

#include <string.h>

static const char * SDP_ORIGIN_PATTERN = "(.*) +(.*) +(.*) +(.*) +(.*) +(.*)";
static const char * SDP_CONNECTION_DATA_PATTERN = "(.*) +(.*) +(.*)";
static const char * SDP_SESSION_TIME_PATTERN = "(.*) +(.*)";
static const char * SDP_RTPMAP_PATTERN("rtpmap:(.+) +([0-9A-Za-z]+)/([0-9]+)/?([0-9])?");
static const char * SDP_FMTP_H264_PATTERN("fmtp:(.+) +packetization-mode=([0-2]);.*sprop-parameter-sets=([A-Za-z0-9+/=]+),([A-Za-z0-9+/=]+)");
static const char * SDP_FMTP_H265_PATTERN("fmtp:(.+) +packetization-mode=([0-2]);.*sprop-vps=([A-Za-z0-9+/=]+);.*sprop-sps=([A-Za-z0-9+/=]+);.*sprop-pps=([A-Za-z0-9+/=]+)");
static const char * SDP_CONTROL_PATTERN("control:(.+)");

SDPData::~SDPData() 
{
}

void SDPData::parse(string &sdp)
{
	MyRegex regex;
	string pattern = "([a-zA-Z])=(.*)";
    string key("");
    string value("");
	list<string> group;
    /* start parse session info first as default */
    bool sessionInfo = true, mediaInfo = false, timeInfo = false;
    stringstream ssTmp;
    SDPMediaInfo * currentMediaInfo = NULL;

	// int debugCount=0;
    while(regex.RegexLine(&sdp, &pattern, &group)) {
		// cout << "debug: Count " << debugCount++ << endl;
        if(group.empty()) {
            break;
        }
        group.pop_front();
        group.pop_front();
        key.assign(group.front()); group.pop_front();
        value.assign(group.front()); group.pop_front();
        /* 's': session info start flag */
        /* 'm': media info start flag */
        /* 't': time info start flag */
        if(key == "s") {
            sessionInfo = true;
            mediaInfo = false;;
            timeInfo = false;
        } else if(key == "m") {
            sessionInfo = false;
            mediaInfo = true;;
            timeInfo = false;
        } else if(key == "t") {
            sessionInfo = false;
            mediaInfo = false;
            timeInfo = true;
        } 

        if(sessionInfo) {
            if("s" == key) {
                sessionName.assign(value);
            } else if("v" == key) {
                ssTmp.clear();
                ssTmp.str("");
                ssTmp << value;
                ssTmp >> sdpVersion;
            } else if("o" == key) {
                if(regex.Regex(value.c_str(), SDP_ORIGIN_PATTERN, &group)) {
                    group.pop_front();
                    ssTmp.clear();
                    ssTmp.str("");
                    sdpOriginStruct.userName.assign(group.front());
                    ssTmp << group.front(); group.pop_front();
                    ssTmp >> sdpOriginStruct.sessionId;
                    ssTmp << group.front(); group.pop_front();
                    ssTmp >> sdpOriginStruct.version;
                    sdpOriginStruct.networkType.assign(group.front());group.pop_front();
                    sdpOriginStruct.addressType.assign(group.front());group.pop_front();
                    sdpOriginStruct.address.assign(group.front());group.pop_front();
                }
                if(regex.Regex(value.c_str(), SDP_CONNECTION_DATA_PATTERN, &group)) {
                    group.pop_front();
                    sdpConnectionData.networkType.assign(group.front());group.pop_front();
                    sdpConnectionData.addressType.assign(group.front());group.pop_front();
                    sdpConnectionData.address.assign(group.front());group.pop_front();
                }
            }
        }
        if(timeInfo) {
            if("t" == key) {
                if(regex.Regex(value.c_str(), SDP_SESSION_TIME_PATTERN, &group)) {
                    group.pop_front();
                    ssTmp.clear();
                    ssTmp.str("");
                    ssTmp << group.front(); group.pop_front();
                    ssTmp >> sdpSessionTime.startTime;
                    ssTmp << group.front(); group.pop_front();
                    ssTmp >> sdpSessionTime.stopTime;
                }
            }
        }
        if(mediaInfo) {
            if("m" == key) {
                ssTmp.clear();
                ssTmp.str(value);
                string tmp;
                ssTmp >> tmp;
                currentMediaInfo = &mediaInfoMap[tmp];
                currentMediaInfo->mediaType.assign(tmp);
                ssTmp >> currentMediaInfo->ports;
                ssTmp >> currentMediaInfo->transProt;
                ssTmp >> tmp;
                while(ssTmp && !tmp.empty()) {
                    stringstream ss;
                    int payloadId;
                    ss << tmp;
                    ss >> payloadId;
                    map<SDP_ATTR_ENUM, string> * fmtMapTmp = &currentMediaInfo->fmtMap[payloadId];
                    (*fmtMapTmp)[MEDIA_TYPE_NAME] = currentMediaInfo->mediaType;
					ssTmp >> tmp;
                }
                
            } else if("a" == key) {
                group.clear();
				// cout << "debug: mt=" << currentMediaInfo->mediaType << ",line=" << value << endl;
                if(currentMediaInfo != NULL && regex.Regex(value.c_str(), SDP_RTPMAP_PATTERN, &group)) {
                    group.pop_front();
                    stringstream ss;
                    int payloadId;
                    ss << group.front(); group.pop_front();
                    ss >> payloadId;
                    map<int, map<SDP_ATTR_ENUM, string>>::iterator it = currentMediaInfo->fmtMap.find(payloadId);
                    if(it != currentMediaInfo->fmtMap.end()) {
                        it->second[CODEC_TYPE] = group.front(); group.pop_front();
                        it->second[TIME_RATE] = group.front(); group.pop_front();
                        it->second[CHANNEL_NUM] = "1"; // default 1 channel
                        if(!group.empty()) {
                            it->second[CHANNEL_NUM] = group.front(); group.pop_front();
                        }
                    }
                } else if(currentMediaInfo != NULL && "video" == currentMediaInfo->mediaType && regex.Regex(value.c_str(), SDP_FMTP_H264_PATTERN, &group)) {
					cout << "SDP_FMTP_H264_PATTERN" << endl;
                    group.pop_front();
                    stringstream ss;
                    int payloadId;
                    ss << group.front(); group.pop_front();
                    ss >> payloadId;
                    map<int, map<SDP_ATTR_ENUM, string>>::iterator it = currentMediaInfo->fmtMap.find(payloadId);
                    if(it != currentMediaInfo->fmtMap.end()) {
                        it->second[PACK_MODE] = group.front(); group.pop_front();
                        it->second[SPS] = group.front(); group.pop_front();
                        it->second[PPS] = group.front(); group.pop_front();
                    }
                } else if(currentMediaInfo != NULL && "video" == currentMediaInfo->mediaType && regex.Regex(value.c_str(), SDP_FMTP_H265_PATTERN, &group)) {
                    group.pop_front();
                    stringstream ss;
                    int payloadId;
                    ss << group.front(); group.pop_front();
                    ss >> payloadId;
                    map<int, map<SDP_ATTR_ENUM, string>>::iterator it = currentMediaInfo->fmtMap.find(payloadId);
                    if(it != currentMediaInfo->fmtMap.end()) {
                        it->second[PACK_MODE] = group.front(); group.pop_front();
                        it->second[VPS] = group.front(); group.pop_front();
                        it->second[SPS] = group.front(); group.pop_front();
                        it->second[PPS] = group.front(); group.pop_front();
                    }
                } else if(currentMediaInfo != NULL && regex.Regex(value.c_str(), SDP_CONTROL_PATTERN, &group)) {
                    group.pop_front();
                    currentMediaInfo->controlURI = group.front(); group.pop_front();
                }

            }
        }
    }
}
