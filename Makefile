CC = gcc
CXX = g++
INCLUDE =

# Optimize
CFLAGS = -Wall -Werror $(INCLUDE) -O2 -ffast-math -fomit-frame-pointer -fstrict-aliasing -g -DDEBUG `sdl-config --cflags` `freetype-config --cflags`
CXXFLAGS = $(CFLAGS)

LIBS = `sdl-config --libs` -lGLU `freetype-config --libs` -lpng

SOURCES = FurnitureGame.cpp Level.cpp house.cpp furniture.cpp FontRect.cpp \
font.cpp AllLevels.cpp

OBJECTS = $(subst .cpp,.o,$(SOURCES))

.PHONY: clean

all: furniture

furniture: $(OBJECTS)
	$(CXX) $(CFLAGS) $(LIBS) $(OBJECTS) -o furniture

clean:
	rm -f $(OBJECTS) furniture

%.o: %.c %.d %.h
%.d: %.c %.h
	$(CPP) -MM $(CFLAGS) $*.cpp > $*.d

