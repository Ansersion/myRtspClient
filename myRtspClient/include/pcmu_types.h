//   Copyright 2015-2018 Ansersion
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

#ifndef PCMU_TYPES_H
#define PCMU_TYPES_H

#include <string>
#include <stdint.h>
#include <audio_type_base.h>

class PCMUTypeBase : public AudioTypeBase
{
	public:
		PCMUTypeBase() {Name.assign("PCMUTypeBase");};
		virtual ~PCMUTypeBase() {};

	public:
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size) { return 0;};
		virtual int GetFlagOffset(const uint8_t * RTPPayload) { return -1; };

	// protected:
	// 	std::string Name;
};


class PCMU_Audio : public PCMUTypeBase
{
	public:
		PCMU_Audio() { Name.assign("PCMU_Audio"); };
		virtual ~PCMU_Audio() {};
		
	public:
		virtual size_t CopyData(uint8_t * buf, uint8_t * data, size_t size);
		virtual int GetFlagOffset(const uint8_t * RTPPayload);
};

#endif
