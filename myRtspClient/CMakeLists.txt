cmake_minimum_required(VERSION 2.8)

project(myRtspClient)

#g++ common_example.cpp -I ../myRtspClient/include ../myRtspClient/libmyRtspClient.a -I ../third_party/JRTPLIB/src ../third_party/JRTPLIB/src/libjrtp.a

#add_subdirectory(../third_party/Base64_live555 base64_live555)
#add_subdirectory(../third_party/md5 md5)
#file(GLOB SOURCES "../third_party/Base64_live555/*.cpp")
#add_library(libbase64_live555
#            STATIC
#            ${SOURCES})
#target_include_directories(libbase64_live555 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/Base64_live555/include)
#
#file(GLOB SOURCES "../third_party/md5/*.cpp")
#add_library(libmd5
#            STATIC
#            ${SOURCES})
#target_include_directories(libmd5 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/md5/include)

#add_subdirectory(../third_party/JRTPLIB jrtplib)
#add_dependencies(jrtplib)

add_subdirectory(../third_party/JRTPLIB jrtplib)
#ExternalProject_Add(jrtplib SOURCE_DIR ../third_party/JRTPLIB BINARY_DIR jrtplib_ INSTALL_DIR jrtplib)
include_directories(include)
include_directories(../third_party/Base64_live555/include)
include_directories(../third_party/md5/include)
include_directories(../third_party/JRTPLIB/src)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/jrtplib/src)

file(GLOB SOURCES "*.cpp" "../third_party/Base64_live555/*.cpp" "../third_party/md5/*.cpp") 

#add_library(libmd5 STATIC IMPORTED)
#SET_PROPERTY(TARGET libmd5 PROPERTY IMPORTED_LOCATION md5/libmd5.a)
#SET_PROPERTY(TARGET libmd5 PROPERTY POSITION_INDEPENDENT_CODE ON)

#add_library(libbase64_live555 STATIC IMPORTED)
#SET_PROPERTY(TARGET libbase64_live555 PROPERTY IMPORTED_LOCATION base64_live555/libbase64_live555.a)
#SET_PROPERTY(TARGET libbase64_live555 PROPERTY POSITION_INDEPENDENT_CODE ON)

#add_library(libjrtp STATIC IMPORTED) 
#SET_PROPERTY(TARGET libjrtp PROPERTY IMPORTED_LOCATION jrtplib/src/libjrtp.a)
#SET_PROPERTY(TARGET libjrtp PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(myRtspClient SHARED ${SOURCES})
add_dependencies(myRtspClient jrtplib-shared)

#target_link_libraries(myRtspClient)

#target_link_libraries(myRtspClient libmd5 libbase64_live555)# libjrtp)
#install(TARGETS myRtspClient DESTINATION /usr/lib)
