#include "myRegex.h"
#include <stdio.h>
#include <stdlib.h>

MyRegex::MyRegex():
	pReg(NULL), pMatch(NULL)
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
