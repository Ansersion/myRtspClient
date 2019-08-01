project(myRtspClient)

include_directories(third_party/Base64_live555/include)
include_directories(third_party/md5/include)
include_directories(third_party/JRTPLIB/src)
include_directories(include)
include_directories(myRtspClient/include)
include_directories(third_party/JTHREAD/src)


add_subdirectory(third_party/JRTPLIB _JTRPLIB)
include_directories(${PROJECT_BINARY_DIR}/_JTRPLIB/src)


file(GLOB SOURCES "myRtspClient/*.cpp" "third_party/Base64_live555/*.cpp" "third_party/md5/*.cpp" "third_party/JTHREAD/*.cpp"  "third_party/JTHREAD/src*.cpp"  "third_party/JTHREAD/jthread/*.cpp")

add_library(myRtspClient SHARED ${SOURCES})
add_library(myRtspClient-static STATIC ${SOURCES})
target_link_libraries(myRtspClient jrtplib-shared)
target_link_libraries(myRtspClient-static jrtplib-static)
