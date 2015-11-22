.PHONY: clean test

EXECUTABLE=qmread

CC=g++ 

CFLAGS=-O2 -Wall -Wno-unused-result -std=c++11 -ggdb 
LDFLAGS=

SOURCES=qmread.cpp lmfile.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=hello


all: clean $(SOURCES) $(EXECUTABLE)

qmread: $(EXECUTABLE).cpp
	$(CC) $^ -o $@


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


test:
	cppcheck $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) *.o

install:
	echo "install mesycut to /usr/local/bin"
	sudo cp qmread /usr/local/bin
