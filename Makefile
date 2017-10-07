CC = g++
CFLAGS = -g -Wall
LDFLAGS= 
SOURCES=main.cpp lexer.cpp parser.cpp interpreter.cpp ast.cpp tables.cpp visitor.cpp visitor_print.cpp visitor_name_resolution.cpp 
HEADERS=$(SOURCES:.cpp=.h)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sql

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm $(OBJECTS) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
