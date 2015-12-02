#include <regex.h>
#include <list>
#include <string>
#include <string.h>

#define REGEX_BUF_SIZE 	2048

using std::list;
using std::string;

const int REGEX_FAILURE = 0;
const int REGEX_SUCCESS = 1;
class MyRegex
{
	public:
		MyRegex();
		~MyRegex();
		int Regex(const char * str, const char * pattern, list<string> * groups, bool ignore_case);
	protected:
		const char * Substr(const char * str, int pos1, int pos2);
		regex_t * pReg;
		regmatch_t * pMatch;
		char RegexBuf[REGEX_BUF_SIZE];

};
