CXX = g++
INCLUDE_DIR =
INCLUDE_DIR = -I$(shell pwd)
CPPFLAGS += -g -O0 $(INCLUDE_DIR) -Wall
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o, $(SRCS))
THIRD_PATRY := ./third_party

JRTP := $(THIRD_PATRY)/jrtplib-3.9.1/build

JRTP_LIB =
JRTP_LIB += $(JRTP)/src/libjrtp.a

JRTP_INCLUDE =
JRTP_INCLUDE += -I$(JRTP)/src
JRTP_INCLUDE += -I$(JRTP)/../src

CPPFLAGS += $(JRTP_INCLUDE)

CXXFLAGS += $(INCLUDE_DIR)

TARGET_SIMPLE_EXAMPLE := simple_example
SIMPLE_EXAMPLE_CPP 	:= ./example/simple_example.cpp
TARGET_COMPLETE_EXAMPLE := complete_example
COMPLETE_EXAMPLE_CPP 	:= ./example/complete_example.cpp


all: $(TARGET_SIMPLE_EXAMPLE) $(TARGET_COMPLETE_EXAMPLE)

$(TARGET_SIMPLE_EXAMPLE) : $(OBJS) 
	$(CXX) $(CXXFLAGS) $(SIMPLE_EXAMPLE_CPP) -o $@ $(OBJS) $(JRTP_LIB)

$(TARGET_COMPLETE_EXAMPLE) : $(OBJS) 
	$(CXX) $(CXXFLAGS) $(COMPLETE_EXAMPLE_CPP) -o $@ $(OBJS) $(JRTP_LIB)

.depend: $(SRCS) 
	@$(CXX) $(JRTP_INCLUDE) -MM $(SRCS) > $@  		
	@cscope -Rbq
sinclude .depend
sinclude jrtplib

.PHONY : clean jrtplib
clean:
	rm -f $(TARGET) *.o .depend cscope.*

jrtplib:
	@mkdir -p $(JRTP)
	@echo "Compile jrtplib first"
	@sleep 2
	(cd $(JRTP); rm -f Makefile; cmake ..; make)
