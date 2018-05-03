cmake_minimum_required(VERSION 2.8)

project(myRtspClient)

#g++ common_example.cpp -I ../myRtspClient/include ../myRtspClient/libmyRtspClient.a -I ../third_party/JRTPLIB/src ../third_party/JRTPLIB/src/libjrtp.a

add_subdirectory(../third_party/JRTPLIB jrtplib)
include_directories(../third_party/Base64_live555/include)
include_directories(../third_party/md5/include)
include_directories(../third_party/JRTPLIB/src)
include_directories(include)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/jrtplib/src)

file(GLOB SOURCES "*.cpp" "../third_party/Base64_live555/*.cpp" "../third_party/md5/*.cpp") 

add_library(myRtspClient SHARED ${SOURCES})
add_dependencies(myRtspClient jrtplib-shared)
target_link_libraries(myRtspClient jrtplib-shared)