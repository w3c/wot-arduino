CXX = g++
CPPFLAGS =        # put pre-processor settings (-I, -D, etc) here
CXXFLAGS = -Wall  # put compiler settings here
LDFLAGS =         # put linker settings here
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

WebThings.o: WebThings.h JSON.h AvlNode.h
JSON.o : JSON.h AvlNode.h
AvlNode.o: AvlNode.h

