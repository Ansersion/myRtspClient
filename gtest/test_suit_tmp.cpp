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
#include "rtspClient.h"
#include "MediaSession.h"

using namespace std;
// #include "myRtpSession.h"

// TEST(myRegex, Regex_InvalidInput)
// {
//     MyRegex Regex;
// 
// 	// char str[] = "audio 0 RTP/AVP 14 16 18\r\n";
// 	char str[] = "audio 0 RTP/AVP 14\r\n";
// 	char pattern[] = "([a-zA-Z]+) +([0-9/]+) +([A-Za-z/]+) +\\b([0-9]+)\\b";
//     list<string> group;
//     bool IgnoreCase = true;
// 
// 
//     EXPECT_EQ(Regex.Regex(str, pattern, &group, IgnoreCase), MyRegex::REGEX_SUCCESS);
// 
// 	int i = 0;
// 	for(list<string>::iterator it = group.begin(); it != group.end(); it++) {
// 		i++;
// 		cout << i << ": " << *it << endl;
// 	}
// }
