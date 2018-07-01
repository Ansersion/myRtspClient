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
#include "utils.h"

using namespace std;
// #include "myRtpSession.h"

// TEST(myRegex, Regex)
// {
// 	MyRegex Regex;
// 
// 	// char str[] = "audio 0 RTP/AVP 14 16 18\r\n";
// 	char str[] = "WWW-Authenticate:Digest  realm=\"HipcamRealServer\", nonce=\"3b27a446bfa49b0c48c3edb83139543d\"";
// 	char pattern[] = "WWW-Authenticate:Digest +realm=(\"[a-zA-Z_0-9 ]+\"), +nonce=(\"[a-zA-Z0-9]+\")";
// 	list<string> group;
// 	bool IgnoreCase = true;
// 
// 
// 	EXPECT_EQ(Regex.Regex(str, pattern, &group, IgnoreCase), MyRegex::REGEX_SUCCESS);
// 
// 	int i = 0;
// 	for(list<string>::iterator it = group.begin(); it != group.end(); it++) {
// 		i++;
// 		cout << i << ": " << *it << endl;
// 	}
// }

// TEST(MD5, MD5_32)
// {
// 	MyRegex Regex;
// 
// 	// char str[] = "audio 0 RTP/AVP 14 16 18\r\n";
// 	char str[] = "a";
// 	char output[33];
// 	output[32] = '\0';
// 	// char pattern[] = "WWW-Authenticate:Digest +realm=(\"[a-zA-Z_0-9 ]+\"), +nonce=(\"[a-zA-Z0-9]+\")";
// 	// list<string> group;
// 	// bool IgnoreCase = true;
// 	EXPECT_EQ(Md5sum32((void *)str, (unsigned char *)output, 1, 33), 0);
// 
// 
// 	// EXPECT_EQ(Regex.Regex(str, pattern, &group, IgnoreCase), MyRegex::REGEX_SUCCESS);
// 
// 	// int i = 0;
// 	// for(list<string>::iterator it = group.begin(); it != group.end(); it++) {
// 	// 	i++;
// 	// 	cout << i << ": " << *it << endl;
// 	// }
// }
