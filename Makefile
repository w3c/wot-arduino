CXX = g++
CPPFLAGS = -I.       # pre-processor settings (-I, -D, etc)
CXXFLAGS = -Wall -Os -fdata-sections -ffunction-sections  # compiler settings
LDFLAGS = #-Wl,--gc-sections  # linker settings
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
INCLUDES=$(wildcard *.h)
DEPS=$(SOURCES:.cpp=.d)

test: $(OBJECTS)
	$(CXX) -o test $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.cpp.o: $(INCLUDES)
	$(CXX) -D DEBUG $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJECTS) test

demo.o: Arduino.h WebThings.h JSON.h Names.h AvlNode.h NodePool.h
WebThings.o: Arduino.h WebThings.h JSON.h Names.h AvlNode.h NodePool.h
NodePool.o: Arduino.h NodePool.h
JSON.o: Arduino.h JSON.h AvlNode.h Names.h NodePool.h
Names.o: Arduino.h Names.h
AvlNode.o: Arduino.h AvlNode.h NodePool.h
MessageCoder.o: Arduino.h MessageCoder.h
