#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <iostream>
#include <gtest/gtest.h>

#include "myRegex.h"
using namespace std;

TEST(myRegex, Regex_InvalidInput)
{
    MyRegex Regex;

    char str[] = "ansersion";
    char pattern[] = "ansersion";
    list<string> group;
    bool IgnoreCase = true;


    /* int Regex(const char * str, const char * pattern, list<string> * groups, bool ignore_case = false); */
    EXPECT_EQ(Regex.Regex(NULL, pattern, &group, IgnoreCase), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(str, NULL, &group, IgnoreCase), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(str, pattern, NULL, IgnoreCase), MyRegex::REGEX_FAILURE);

    EXPECT_EQ(Regex.Regex(NULL, pattern, &group, !IgnoreCase), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(str, NULL, &group, !IgnoreCase), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(str, pattern, NULL,!IgnoreCase), MyRegex::REGEX_FAILURE);

    /* int Regex(const char * str, const char * pattern, bool ignore_case = false); */
    EXPECT_EQ(Regex.Regex(NULL, pattern, IgnoreCase), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(str, NULL, IgnoreCase), MyRegex::REGEX_FAILURE);

    EXPECT_EQ(Regex.Regex(NULL, pattern, !IgnoreCase), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(str, NULL, !IgnoreCase), MyRegex::REGEX_FAILURE);
}

TEST(myRegex, Regex_RegularInput)
{
    char str[] = "ansersion";
    char str_with_brackets[] = "an(sersion";
    char str_with_new_line[] = "ansersion\nwan_ansersion";
    char pattern[] = "anser(sion)";
    char pattern_with_brackets[] = "an\\(sersion";
    MyRegex Regex;
    list<string> group;

    EXPECT_EQ(Regex.Regex(str, pattern, &group), MyRegex::REGEX_SUCCESS);
    EXPECT_EQ(Regex.Regex(str_with_brackets, pattern_with_brackets, &group), MyRegex::REGEX_SUCCESS);
    EXPECT_EQ(Regex.Regex(str_with_new_line, pattern, &group), MyRegex::REGEX_SUCCESS);


	//### Test anchor tag '\b' ###//
	string AnchorStr1("rtsp://192.168.15.1007/test");
	string AnchorStr2("rtsp://192.168.15.100/test");
	string AnchorPattern("rtsp://([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})\\b");
    EXPECT_EQ(Regex.Regex(AnchorStr1.c_str(), AnchorPattern.c_str(), &group), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(AnchorStr2.c_str(), AnchorPattern.c_str(), &group), MyRegex::REGEX_SUCCESS);

	//### Test single line ###//
	string SingleStr1("\r\n");
	string SingleStr2("\n");
	string SingleStr3("ansersion\r\n");
	string SingleStr4("\r\nansersion");
	string SingleStr5("\nansersion");
	string SingleStr6("ansersion\n");

	string SinglePattern("^(\r\n|\n)$");

    EXPECT_EQ(Regex.Regex(SingleStr1.c_str(), SinglePattern.c_str()), MyRegex::REGEX_SUCCESS);
    EXPECT_EQ(Regex.Regex(SingleStr2.c_str(), SinglePattern.c_str()), MyRegex::REGEX_SUCCESS);

    EXPECT_EQ(Regex.Regex(SingleStr3.c_str(), SinglePattern.c_str()), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(SingleStr4.c_str(), SinglePattern.c_str()), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(SingleStr5.c_str(), SinglePattern.c_str()), MyRegex::REGEX_FAILURE);
    EXPECT_EQ(Regex.Regex(SingleStr6.c_str(), SinglePattern.c_str()), MyRegex::REGEX_FAILURE);
}

TEST(myRegex, RegexLine_RegularInput)
{
    string str = "\
                                 Apache License\n\
                           Version 2.0, January 2004\n\
                        http://www.apache.org/licenses/\n\
			\n\
   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION\n\
   ";
    string LineWillBeMatched("                        http://www.apache.org/licenses/"); 
    string PartWillBeMatched("http://www.apache");
    string GroupWillBeMatched("apache");


    string pattern = "http:.*(apache)";
    MyRegex Regex;
    list<string> group;

    while(Regex.RegexLine(&str, &pattern, &group)) {
	if(!group.empty()) {
	    EXPECT_EQ(true, LineWillBeMatched == group.front());
	    group.pop_front(); // pop the line
	    EXPECT_EQ(true, PartWillBeMatched == group.front());
	    group.pop_front(); // pop the matched part
	    EXPECT_EQ(true, GroupWillBeMatched == group.front());
	    group.pop_front(); // pop the group
	}
    }
}
