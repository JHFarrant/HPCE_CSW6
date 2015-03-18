SHELL=/bin/bash


# get OS name
UNAME := $(shell uname)

CPPFLAGS += -std=c++11 -W -Wall  -g 
CPPFLAGS += -O3
CPPFLAGS += -I include 

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
clean:
<<<<<<< HEAD
	rm bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle bin/compare_puzzle_output
	rm lib/libpuzzler.a
=======
	rm -rf lib/*
	rm -rf bin/*
	rm -rf provider/*.o


>>>>>>> 247f909... merged with Jack's code
