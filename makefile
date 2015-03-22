SHELL=/bin/bash
include marker

# get OS name
UNAME := $(shell uname)

CPPFLAGS += -I include -std=c++11 -W -Wall  -g 

#CPPFLAGS += -O3


TBB_DIR =tbb43_20150209oss
#TBB_DIR =/usr/local/Cellar/tbb/4.3-20141023

TBB_INC_DIR = $(TBB_DIR)/include
TBB_LIB_DIR = $(TBB_DIR)/lib/intel64/gcc4.4
#TBB_LIB_DIR = $(TBB_DIR)/lib


CPPFLAGS += -ltbb -I /usr/include
LDFLAGS += -L /usr/lib

# for OSX
ifeq ($(UNAME), Darwin)
	CPPFLAGS += -framework OpenCL
endif

#for Linux
ifeq ($(UNAME), Linux)
	#CPPFLAGS += -lOpenCL
	LDLIBS = -lm  -lOpenCL
endif


lib/libpuzzler.a : provider/*.cpp provider/*.hpp 
	cd provider && $(MAKE) all

bin/% : src/%.cpp lib/libpuzzler.a provider/CL/setup.cpp
	-mkdir -p bin
	$(CXX)  $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) -Llib -lpuzzler

all : bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle bin/compare_puzzle_output

br : 
	rm -rf provider/CL/*.bin
clean: 
	rm -rf provider/*.o
	rm -rf lib/*
	rm -rf bin/*
	rm -rf provider/CL/*.raw provider/CL/*.bini


marker: Makefile
	@touch $@
	$(MAKE) clean
	$(MAKE)
