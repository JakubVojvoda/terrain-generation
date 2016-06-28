#
# LoD terrain generation and displaying
# Svoboda Petr, Vojvoda Jakub
# 
# 2014
#

##################################################
CFLAGS_COMMON=-pipe -Wall -ggdb
CC=gcc
CXX=g++

##################################################
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
	LIBS=-lopengl32 -lglew32 -lSDLmain -lSDL -lmingw32
	RM=del
else
	USE_GLEE=1
	CFLAGS=$(CFLAGS_COMMON) `sdl-config --cflags` -I. -IGL -DUSE_GLEE
	LIBS=`sdl-config --libs` -lGL
	RM=rm -f
endif

CXXFLAGS=$(CFLAGS)

##################################################
PROJ=terrain.exe 

ifdef USE_GLEE
	DEPS=pgr.o GLee.o trianglemesh.o heightmap.o perlinnoise.o patch.o landscape.o
else
	DEPS=pgr.o trianglemesh.o heightmap.o perlinnoise.o patch.o landscape.o 
endif

.PHONY: all lab1 clean

all: lab1

lab1: $(PROJ)

clean:
	$(RM) $(PROJ) $(DEPS) *~

##################################################
%.exe: %.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.exe: %.cpp
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

##################################################
pgr.o: pgr.cpp pgr.h
trianglemesh.o: trianglemesh.cpp trianglemesh.h
heightmap.o: heightmap.cpp heightmap.h
perlinnoise.o: perlinnoise.cpp perlinnoise.h
patch.o: patch.cpp patch.h
landscape.o: landscape.cpp landscape.h

# PROJ

terrain.exe: terrain.cpp $(DEPS)
