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

TEST(MD5, MD5_RegularInput)
{
	char str[] = "a";
	char output[33];
	char a_md5[] = "0cc175b9c0f1b6a831c399e269772661";

	output[32] = '\0';
	EXPECT_EQ(Md5sum32((void *)str, (unsigned char *)output, 1, 33), 0);
	EXPECT_EQ(strncmp(output, a_md5, 32), 0);
}
