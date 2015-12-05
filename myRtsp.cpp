#include <iostream>
#include "rtspClient.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    RtspClient rc;
    rc.DoOPTIONS("rtsp://10.82.15.32/1/h264");
    return 0;
}
