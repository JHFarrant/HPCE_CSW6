HPCE 2014 CW6
=============

- by David Fofana (df611) and Jack Farrant
- Issued: Sun 8th March
- Due: Sun 22nd March, 23:59


Planning
====

Aim 
----

We knew from the coursework specifications that we had to speed up the
execution time of the execute() method of all instances of puzzle 

The first step before implementing any code involved profiling these
functions to determine which ones were the slowest. 

Our initial tests showed that the matrix exponent puzzle had the worst scaling
performance (starts becoming slow for a scale of 700 on intel i7 2.7GHz dual
core CPU) and option explicit had the best (starts becoming slow at
a scale of 10000 on the same machine)

We thus narrowed our aim to making the slowest programs faster first while
allowing time to try to speed up each of them (we didn't ignore the fact that
some might already fast but could easily be made faster)

Splitting the work
------------------

As previously mentioned, we prioritised the work based on the puzzles that gave 
the worst scaling performance. It was no suprise that these puzzles would be the 
most time consuming to re-implement. That's another reason why we wanted to do
these first.

It was found that the life puzzle had the second worst scaling performance so
David started working on it while Jack worked on matrix exponent. We decided
to implement both in OpenCL as there was a big window for scaling that could 
be improved.

From there, any person that finished one puzzle could move on to the next 
available one.


Performance analysis
===================================

The Xcode instruments tool showed the total time spent in all the function calls
in a program of choice and also showed the most called functions. This helped us
quickly pinpoint which parts had to be sped up and saved us a lot of time.


Testing methodology
===================

Testing the programs always involved checking first if they gave the correct 
output for different scales after which they were timed. The tests were 
performed on 4 different machines: 

- David's MacBook Pro (Intel core 2 duo i7 @ 2.7GhZ , No GPU)
- AWS micro instance
- DoC Graphic workstation with NVIDIA GTX Titan graphics card
- AWS GPU instance

The last two were for OpenCL of course.

Verification
------------

We verified our programs by running the run_puzzle program which executes 
both the reference and the user versions and compares them. 

As well as looking at the puzzles printout on the console, debugging 
was done on Xcode. 

Timing
-------

We timed the puzzles by changing the run_puzzle C++ program to use std::clock
and output the time difference but also with the unix time command.

The time_execute.sh script was used to specifically time the execution of only
one version of a puzzle. For example, if we want to time the reference version 
of circuit_sim for a scale of 1000, it will run the following command:

time(./bin/create_puzzle_input life 1000 0 | ./bin/execute_puzzle 1 0  > /dev/null)

Design
=======

The general approach used to improve performance was to look out for opportunities
of parallelism (i.e for loops, recursion), redundant calculations. In the case of 
for loops, we would try to parallelize them depending on whether they had loop
carried dependencies or not.

Life
----

This was implemented in OpenCL.

Matrix exponent
---------------


String Search
-------------

Circuit sim 
-----------

This was implemented in TBB.

Option explicit 
---------------

This was implemented in TBB.
median bits
-----------
