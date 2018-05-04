cmake_minimum_required(VERSION 2.8)

project(examples)

include_directories(include)
file(GLOB SOURCES "*.cpp")

add_library(base64_live555 STATIC ${SOURCES})
install(TARGETS base64_live555 DESTINATION .)
