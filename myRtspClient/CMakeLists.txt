cmake_minimum_required(VERSION 2.8)

project(myRtspClient)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/Base64_live555 base64_live555)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/md5 md5)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/JRTPLIB jrtplib)

include_directories(include)
include_directories(../third_party/Base64_live555/include)
include_directories(../third_party/md5/include)
include_directories(jrtplib/src)
include_directories(../third_party/JRTPLIB/src)
file(GLOB SOURCES "*.cpp")

add_library(myRtspClient STATIC ${SOURCES})
#install(TARGETS myRtspClient DESTINATION /usr/lib)
