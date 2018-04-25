cmake_minimum_required(VERSION 2.8)

project(Base64_live555)

include_directories(include)
file(GLOB SOURCES "*.cpp")

add_library(base64_live555 STATIC ${SOURCES})
#install(TARGETS myRtspClient DESTINATION /usr/lib)
