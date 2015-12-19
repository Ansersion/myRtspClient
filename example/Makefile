CXX = g++
INCLUDE_DIR =
INCLUDE_DIR = -I$(shell pwd)
INCLUDE_DIR = -I../
CPPFLAGS += -g -O0 $(INCLUDE_DIR) -Wall
SRCS := $(wildcard ../*.cpp)
OBJS := $(patsubst %.cpp,%.o, $(SRCS))
THIRD_PATRY := ../third_party

JRTP := $(THIRD_PATRY)/jrtplib-3.9.1/build

JRTP_LIB =
JRTP_LIB += $(JRTP)/src/libjrtp.a

JRTP_INCLUDE =
JRTP_INCLUDE += -I$(JRTP)/src
JRTP_INCLUDE += -I$(JRTP)/../src

CPPFLAGS += $(JRTP_INCLUDE)

CXXFLAGS += $(INCLUDE_DIR)

TARGET_SIMPLE_EXAMPLE := simple_example
SIMPLE_EXAMPLE_CPP 	:= simple_example.cpp
TARGET_COMPLETE_EXAMPLE := complete_example
COMPLETE_EXAMPLE_CPP 	:= complete_example.cpp


all: $(TARGET_SIMPLE_EXAMPLE) $(TARGET_COMPLETE_EXAMPLE)

$(TARGET_SIMPLE_EXAMPLE) : $(OBJS) $(SIMPLE_EXAMPLE_CPP)
	$(CXX) $(CXXFLAGS) $(SIMPLE_EXAMPLE_CPP) -o $@ $(OBJS) $(JRTP_LIB)

$(TARGET_COMPLETE_EXAMPLE) : $(OBJS) $(COMPLETE_EXAMPLE_CPP)
	$(CXX) $(CXXFLAGS) $(COMPLETE_EXAMPLE_CPP) -o $@ $(OBJS) $(JRTP_LIB)


.PHONY : clean jrtplib
clean:
	rm -f $(TARGET) *.o .depend cscope.*

