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
	CPPFLAGS += -lOpenCL
endif


lib/libpuzzler.a : provider/*.cpp provider/*.hpp
	cd provider && $(MAKE) all

bin/% : src/%.cpp lib/libpuzzler.a
	-mkdir -p bin
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) -Llib -lpuzzler

all : bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle bin/compare_puzzle_output

clean:
	rm bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle bin/compare_puzzle_output
	rm lib/libpuzzler.a

