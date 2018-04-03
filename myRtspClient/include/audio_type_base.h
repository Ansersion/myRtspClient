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

#ifndef AUDIO_TYPE_BASE_H
#define AUDIO_TYPE_BASE_H

#include <string>
#include <stdint.h>

class AudioTypeBase
{
	public:
		AudioTypeBase() {Name.assign("AudioTypeBase");};
		virtual ~AudioTypeBase() {};

	public:
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) = 0;
		virtual int GetFlagOffset(const uint8_t * RTPPayload) = 0;

	protected:
		std::string Name;
};

#endif
