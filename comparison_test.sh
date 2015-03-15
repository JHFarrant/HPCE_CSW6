#!/bin/bash

# this script compares the reference and user results of executing a specific puzzle 

REF="ref_out.txt"  #reference filename
USER="user_out.txt"   #user filename
CREATE_LL=0; # create_puzzle_input log level
EX_LL=4; # execute_puzzle log level
COMP_LL=4; # compare_puzzle_output log level
PUZZLE="life"  # puzzle to test 
SCALE=100 # problem size (size of)



./bin/create_puzzle_input $PUZZLE $SCALE $CREATE_LL | tee >(./bin/execute_puzzle 1 $EX_LL  > $REF) \
						    | tee >(./bin/execute_puzzle 0 $EX_LL  > $USER) > /dev/null

./bin/compare_puzzle_output $REF $USER $COMP_LL
