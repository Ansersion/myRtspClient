CXX = g++
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

TARGET := myRtsp
$(TARGET) : $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(JRTP_LIB)

.depend: $(SRCS) 
	@$(CXX) $(JRTP_INCLUDE) -MM $(SRCS) > $@  		
	@cscope -Rbq
sinclude .depend

.PHONY : clean
clean:
	rm -f $(TARGET) *.o .depend cscope.*
