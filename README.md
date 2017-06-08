myRtspClient
================================================================================

This package contains a RTSP Client libraries which is based on JRTPLIB.
Under the license of Apache License V2.0.

Linux/Unix installation
--------------------------------------------------------------------------------
Now, myRtspClient installation is not supported. You can only compile the library
./myRtspClient/libmyRtspClient.a. And the headers is in ./myRtspClient/include.

Compiling based on different os-platform, you NEED to configure the
config.<os-platform> file. 
For example: 
Compiling based on x86, which configure use file 'config.linux':
 	
 	$ ./genMakefiles linux
 	
 	$ make

(Configure your cross compile tool in 'config.armlinux' first)
Compiling based on armlinux, which configure use file 'config.armlinux':
	
	$ ./genMakefiles armlinux
	
	$ make

Features
--------------------------------------------------------------------------------
Video Codec Supported:

	H264
	H265

Audio Codec Supported:

	MPA

Cast Method Supported:
	Unicast
	
Info
--------------------------------------------------------------------------------
Ansersion 	ansersion@sina.com
中文教程 	http://www.cnblogs.com/ansersion/p/6959690.html
