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

#ifndef MY_REGEX_H
#define MY_REGEX_H

#include <regex.h>
#include <list>
#include <string>
#include <string.h>

#define REGEX_BUF_SIZE 	2048

using std::list;
using std::string;

class MyRegex
{
	public:
		static const int REGEX_FAILURE;
		static const int REGEX_SUCCESS;

	public:
		MyRegex();
		~MyRegex();
		int Regex(const char * str, const char * pattern, list<string> * groups, bool ignore_case = false);
		int Regex(const char * str, const char * pattern, bool ignore_case = false);
		/* Imitate the regex use of PERL */
		/* Matching line by line */
		int RegexLine(string * str, string * pattern, list<string> * groups, bool ignore_case = false);
		int RegexLine(string * str, string * pattern, bool ignore_case = false);

	protected:
		const char * Substr(const char * str, int pos1, int pos2);
		regex_t * pReg;
		regmatch_t * pMatch;
		string::size_type NextPos;
		string * LastStr;
		char RegexBuf[REGEX_BUF_SIZE];

};

#endif
