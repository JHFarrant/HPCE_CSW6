SHELL=/bin/bash

CPPFLAGS += -std=c++11 -W -Wall  -g 
CPPFLAGS += -O3
CPPFLAGS += -I ../include -I CL

#TBB_DIR =../tbb43_20150209oss

#TBB_INC_DIR = $(TBB_DIR)/include
#TBB_LIB_DIR = $(TBB_DIR)/lib/intel64/gcc4.4


CPPFLAGS += -ltbb -I /usr/include
LDFLAGS += -L /usr/lib

puzzles.o : *.hpp 
#CL/*.cpp 
# CL/setup.cpp 

../lib/libpuzzler.a : puzzles.o 
	-mkdir -p ../lib
	ar cr ../lib/libpuzzler.a puzzles.o

all : ../lib/libpuzzler.a
