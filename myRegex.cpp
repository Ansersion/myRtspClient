#include "myRegex.h"
#include <stdio.h>
#include <stdlib.h>

const int MyRegex::REGEX_FAILURE = 0;
const int MyRegex::REGEX_SUCCESS = 1;

MyRegex::MyRegex():
	pReg(NULL), pMatch(NULL), NextPos(string::npos), LastStr(NULL)
{
	memset(RegexBuf, 0, REGEX_BUF_SIZE);
}

MyRegex::~MyRegex()
{
	if(pReg) delete pReg;
	if(pMatch) delete pMatch;
}

int MyRegex::Regex(const char * str, const char * pattern, list<string> * groups, bool ignore_case)
{
	if(!str) return REGEX_FAILURE;
	if(!pattern) return REGEX_FAILURE;
	if(!groups) return REGEX_FAILURE;

	string StrPattern(pattern);
	char ErrBuf[1024]; // use "1024" at random
	int GroupNum = 0;
	string::size_type GroupPos = 0;

	GroupNum++; // the whole pattern is the 'first group'
	while((GroupPos = StrPattern.find("(", GroupPos)) != string::npos) GroupNum++, GroupPos++;
	while((GroupPos = StrPattern.find("\\(", GroupPos)) != string::npos) GroupNum--, GroupPos++;
	
	if(GroupNum < 0) {
		printf("REGEX INTERNAL PANIC, NEED DEBUGING(Regex)");
		return REGEX_FAILURE;
	}
	
	pReg = (regex_t *)calloc(1, sizeof(regex_t));
	pMatch = (regmatch_t *)calloc(GroupNum, sizeof(regmatch_t));

	int RegFlags = REG_EXTENDED;
	ignore_case && (RegFlags |= REG_ICASE);

	int ErrNo = regcomp(pReg, pattern, REG_EXTENDED);
	if(ErrNo != 0) {
		regerror(ErrNo, pReg, ErrBuf, sizeof(ErrBuf));
		printf("Regex Error: %s\n", ErrBuf);
		return REGEX_FAILURE;
	}
	ErrNo = regexec(pReg, str, GroupNum, pMatch, 0);
	if(ErrNo != REG_NOERROR) {
		regfree(pReg);free(pReg);free(pMatch);
		pReg = NULL; pMatch = NULL;
		return REGEX_FAILURE;
	} 

	groups->clear();
	// printf("GroupNum: %d\n", GroupNum); //###Debug###//
	for(int i = 0; i < GroupNum; i++) {
		const char * pStr;
		if((pStr = Substr(str, pMatch[i].rm_so, pMatch[i].rm_eo)) != NULL) {
			groups->push_back(pStr);
		}
	}
	regfree(pReg);free(pReg);free(pMatch);
	pReg = NULL; pMatch = NULL;
	return REGEX_SUCCESS;
}

int MyRegex::Regex(const char * str, const char * pattern, bool ignore_case)
{
    list<string> tmp;
    return Regex(str, pattern, &tmp, ignore_case);
}

int MyRegex::RegexLine(string * str, string * pattern, list<string> * groups, bool ignore_case)
{
    const int EndOfString = 0;
    const int StillInString = 1;
    string::size_type TmpPos = 0;
    string MatchedLine("");

    if(LastStr != str) {
	LastStr = str;
	NextPos = 0;
    }
    if(string::npos == NextPos) return EndOfString;
    if(!LastStr || !pattern || !groups) return EndOfString;
    TmpPos = LastStr->find("\n", NextPos);
    groups->clear();
    if(TmpPos != string::npos) {
	MatchedLine.assign(LastStr->substr(NextPos, TmpPos - NextPos));
	NextPos = ++TmpPos;
    } else {
	MatchedLine.assign(LastStr->substr(NextPos));
	NextPos = string::npos;
    }
    if(!Regex(MatchedLine.c_str(), pattern->c_str(), groups, ignore_case)) {
	groups->clear();
    } else {
	groups->push_front(MatchedLine);
    }
    return StillInString;
}

int MyRegex::RegexLine(string * str, string * pattern, bool ignore_case)
{
    list<string> tmp;
    return RegexLine(str, pattern, &tmp, ignore_case);
}

/********************/
/* proteced method */
const char * MyRegex::Substr(const char * str, int pos1, int pos2)
{
	if(!str) return NULL;

	// printf("%d:%d\n", pos1, pos2); //###Debug###//
	int Len = strlen(str);
	int SubLen = pos2 - pos1;

	if(pos1 >= 0 && pos1 <= Len &&
	pos2 >= 0 &&  pos2 <= Len && 
	SubLen >= 0 && SubLen < REGEX_BUF_SIZE) {
		strncpy(RegexBuf, &str[pos1], SubLen);
		RegexBuf[SubLen] = '\0';
		return RegexBuf;
	} else {
		return NULL;
	}

}
