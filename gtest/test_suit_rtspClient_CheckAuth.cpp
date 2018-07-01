//   Copyright 2015-2018 Ansersion
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

#include "rtspClient.h"

using namespace std;

TEST(CHECK_AUTH, CHECK_AUTH)
{
	string RtspUri("rtsp://192.168.81.157:8554/ansersion");
    RtspClient Client;

	Client.SetUsername("Ansersion");
	Client.SetPassword("AnsersionPassword");
	EXPECT_EQ(Client.DoDESCRIBE(RtspUri), RTSP_NO_ERROR);
}
