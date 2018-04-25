cmake_minimum_required(VERSION 2.8)

project(md5)

include_directories(include)
file(GLOB SOURCES "*.cpp")

add_library(md5 STATIC ${SOURCES})
#install(TARGETS myRtspClient DESTINATION /usr/lib)
