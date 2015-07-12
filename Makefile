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

WebThings.o: Arduino.h WebThings.h Registry.h JSON.h AvlNode.h NodePool.h
Registry.o: Arduino.h NodePool.h WebThings.h Registry.h
NodePool.o: Arduino.h NodePool.h
JSON.o: Arduino.h JSON.h AvlNode.h HashTable.h NodePool.h
HashTable.o: Arduino.h HashTable.h
AvlNode.o: Arduino.h AvlNode.h NodePool.h
MessageCoder.o: Arduino.h MessageCoder.h
