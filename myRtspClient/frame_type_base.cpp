//   Copyright 2019 Ansersion
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

#include "frame_type_base.h"
#include "nalu_types_h264.h"
#include "nalu_types_h265.h"
#include "pcmu_types.h"
#include <string.h>

using std::string;


FrameTypeBase * FrameTypeBase::CreateNewFrameType(string &EncodeType)
{
    FrameTypeBase * frame_type_base = NULL;
    if(0 == EncodeType.compare(NALUTypeBase_H264::ENCODE_TYPE)) {
        frame_type_base = new NALUTypeBase_H264();
    } else if(0 == EncodeType.compare(NALUTypeBase_H265::ENCODE_TYPE)) {
        frame_type_base = new NALUTypeBase_H265();
    } else if(0 == EncodeType.compare(PCMU_Audio::ENCODE_TYPE)) {
        frame_type_base = new PCMU_Audio();
    }

    return frame_type_base;
}


void FrameTypeBase::DestroyFrameType(FrameTypeBase * frameTypeBase)
{
    if(!frameTypeBase) {
        delete frameTypeBase;
        frameTypeBase = NULL;
    }
}
