cmake_minimum_required(VERSION 2.8)

project(myRtspClient)

add_subdirectory(../third_party/Base64_live555 base64_live555)
add_subdirectory(../third_party/md5 md5)
add_subdirectory(../third_party/JRTPLIB jrtplib)

include_directories(include)
include_directories(../third_party/Base64_live555/include)
include_directories(../third_party/md5/include)
include_directories(jrtplib/src)
include_directories(../third_party/JRTPLIB/src)

file(GLOB SOURCES "*.cpp")

add_library(myRtspClient STATIC ${SOURCES})

add_library(libmd5 STATIC IMPORTED)
SET_PROPERTY(TARGET libmd5 PROPERTY IMPORTED_LOCATION ./md5)

add_library(libbase64_live555 STATIC IMPORTED)
SET_PROPERTY(TARGET libbase64_live555 PROPERTY IMPORTED_LOCATION ./base64_live555)

add_library(libjrtp STATIC IMPORTED)
SET_PROPERTY(TARGET libjrtp PROPERTY IMPORTED_LOCATION ./jrtplib/src)

target_link_libraries(myRtspClient libmd5 libbase64_live555 libjrtp)
#install(TARGETS myRtspClient DESTINATION /usr/lib)
