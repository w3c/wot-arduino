CXX = g++
CPPFLAGS =        # pre-processor settings (-I, -D, etc)
CXXFLAGS = -Wall -Os -fdata-sections -ffunction-sections  # compiler settings
LDFLAGS = #-Wl,--gc-sections  # linker settings
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
INCLUDES=$(wildcard *.h)
DEPS=$(SOURCES:.cpp=.d)

test: $(OBJECTS)
	$(CXX) -o test $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.cpp.o: $(INCLUDES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJECTS) test

WebThings.o: core.h WebThings.h JSON.h AvlNode.h
JSON.o: core.h JSON.h AvlNode.h HashTable.h
HashTable.o: core.h HashTable.h
AvlNode.o: core.h AvlNode.h
MessageCode.o: core.h
