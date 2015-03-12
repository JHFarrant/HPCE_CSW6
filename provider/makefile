SHELL=/bin/bash

# To build Xcode from makefile
#http://hiltmon.com/blog/2013/07/05/xcode-and-the-simple-c-plus-plus-project-structure/

CPPFLAGS += -std=c++11 -W -Wall  -g
CPPFLAGS += -O3
CPPFLAGS += -I ../include

puzzles.o : *.hpp

../lib/libpuzzler.a : puzzles.o
	-mkdir -p ../lib
	ar -cr ../lib/libpuzzler.a puzzles.o

all : ../lib/libpuzzler.a
