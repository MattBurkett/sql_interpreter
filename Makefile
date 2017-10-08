CXX = g++
CXXFLAGS = -g
SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.h)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sql

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm $(OBJECTS) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@
