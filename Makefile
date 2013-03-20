CC=g++
INCDIR=include/
CFLAGS=-c -Wall -I$(INCDIR)
LDFLAGS=-lGL -lfreeimage -lsfml-system -lsfml-window -lsfml-graphics
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=DiffusionCurves

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -g

clean:
	rm $(EXECUTABLE) $(OBJECTS)

.PHONY: all clean
