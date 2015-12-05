CXX = g++
CPPFLAGS += -g -O0 $(INCLUDE_DIR)
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o, $(SRCS))

TARGET := myRtsp
$(TARGET) : $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) 

.depend: $(SRCS) 
	@$(CXX) -MM $(SRCS) > $@  		
	@cscope -Rbq
sinclude .depend

.PHONY : clean
clean:
	rm -f $(TARGET) *.o .depend cscope.*
