cmake_minimum_required(VERSION 2.8)

project(myRtspClient)

add_subdirectory(../third_party/jrtplib-3.9.1)
add_subdirectory(../third_party/Base64_live555)
add_subdirectory(../third_party/md5)

include_directories(include)
file(GLOB SOURCES "*.cpp")

add_library(myRtspClient STATIC ${SOURCES})
#install(TARGETS myRtspClient DESTINATION /usr/lib)
