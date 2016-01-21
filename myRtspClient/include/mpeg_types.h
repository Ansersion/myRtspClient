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

/*******************************/
/* More info refer to RFC 2250 */
/*******************************/

#ifndef MPEG_TYPES_H
#define MPEG_TYPES_H

#include <string>
#include <stdint.h>

#define MPEG_AUDIO_RTP_HEADER_SIZE 	4

class MPEGTypeBase
{
	public:
		MPEGTypeBase() {Name.assign("MPEGTypeBase");};
		virtual ~MPEGTypeBase() {};

	public:
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) { return 0;};
		virtual int GetFlagOffset(const uint8_t * RTPPayload) { return -1; };

	protected:
		std::string Name;
};

class MPEG_Audio : public MPEGTypeBase
{
	public:
		MPEG_Audio() { Name.assign("MPEG_Audio"); };
		virtual ~MPEG_Audio() {};
		
	public:
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
		virtual int GetFlagOffset(const uint8_t * RTPPayload);
};

#endif
