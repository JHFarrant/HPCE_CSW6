#!/bin/bash

# this script times the user version of execute

#default values
PUZZLE="life"  # puzzle to test 
SCALE=100 # problem size (size of)
LOG=0
REF=0 # by default we run the execute version

if [ -n "$1" ]; then 
	PUZZLE=$1;
fi

if [ -n "$2" ]; then
	SCALE=$2; 
fi

if [ -n "$3" ]; then 
	LOG=$3; 
fi

if [ -n "$4" ]; then 
	REF=$4; 
fi

if [ $LOG -lt 2 ]; then
	time(./bin/create_puzzle_input $PUZZLE $SCALE $LOG | ./bin/execute_puzzle $REF $LOG  > /dev/null)
#fi
else
	time(./bin/create_puzzle_input $PUZZLE $SCALE $LOG | ./bin/execute_puzzle $REF $LOG)
fi 
