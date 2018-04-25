cmake_minimum_required(VERSION 2.8)

project(myRtspClient)
include_directories(include)
file(GLOB SOURCES "*.cpp")

add_library(myRtspClient STATIC ${SOURCES})
#install(TARGETS myRtspClient DESTINATION /usr/lib)
