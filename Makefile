CC=g++
INCDIR=include/
LIBDIR=libs/
CFLAGS=-c -Wall -L$(LIBDIR) -I$(INCDIR)
LDFLAGS=-lGL -lsfml-system -lsfml-window -lsfml-graphics -lSOIL
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
