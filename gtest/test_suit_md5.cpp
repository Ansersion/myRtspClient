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

#define MD5_SIZE 	32
#define MD5_BUF_SIZE 	(MD5_SIZE + sizeof('\0'))

using namespace std;

TEST(MD5, MD5_RegularInput)
{
	char str[] = "a";
	char output[MD5_BUF_SIZE];
	char a_md5[] = "0cc175b9c0f1b6a831c399e269772661";

	output[MD5_SIZE] = '\0';
	EXPECT_EQ(Md5sum32((void *)str, (unsigned char *)output, 1, MD5_BUF_SIZE), 0);
	EXPECT_EQ(strncmp(output, a_md5, MD5_SIZE), 0);
}

TEST(MD5, MD5_RegularInput_RTSP_Authentication)
{
	string realm("LIVE555 Streaming Media");
	string username("Ansersion");
	string password("AnsersionPassword");
	// string nonce("f3c72dd425d78af5886a2e1f97cd00e0");
	string nonce("b8802842a5d9b123cc84f4f9331856ac");
	string cmd("SETUP");
	string uri("rtsp://192.168.81.157:8554/ansersion/track1");
	string tmp("");
	char ha1buf[MD5_BUF_SIZE] = {0};
	char ha2buf[MD5_BUF_SIZE] = {0};
	char habuf[MD5_BUF_SIZE] = {0};
	const char c_ha1[] = "ad68dbfd3e130bcabd2e61d19e5695fd";
	const char c_ha2[] = "1d47c98b00946762aad35c10a7e61736";
	// const char c_ha[] = "02b4d89265cdffcc0414b57fcc6f269c";
	const char c_ha[] = "ceee4678adb535175876d1ba9082d0d8";

	tmp.assign("");
	tmp = username + ":" + realm + ":" + password;
	Md5sum32((void *)tmp.c_str(), (unsigned char *)ha1buf, tmp.length(), MD5_BUF_SIZE);
	// EXPECT_EQ(strncmp(ha1buf, c_ha1, MD5_SIZE), 0);

	tmp.assign("");
	tmp = cmd + ":" + uri;
	Md5sum32((void *)tmp.c_str(), (unsigned char *)ha2buf, tmp.length(), MD5_BUF_SIZE);
	// EXPECT_EQ(strncmp(ha2buf, c_ha2, MD5_SIZE), 0);

	tmp.assign(ha1buf);
	tmp = tmp + ":" + nonce + ":" + ha2buf;
	Md5sum32((void *)tmp.c_str(), (unsigned char *)habuf, tmp.length(), MD5_BUF_SIZE);
	EXPECT_EQ(strncmp(habuf, c_ha, MD5_SIZE), 0);
}
