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
