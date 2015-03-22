HPCE 2014 CW6
=============

- by David Fofana (df611) and Jack Farrant
- Issued: Sun 8th March
- Due: Sun 22nd March, 23:59


Planning
====

We knew from the coursework specifications that we had to speed up the
execution time of the puzzler::Puzzle::Execute() method of all the puzzle
instances.

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
==================

As previously mentioned, we prioritised the work based on the puzzles that gave 
the worst scaling performance. It was no suprise that these puzzles would be the 
most time consuming to re-implement. That's another reason why we wanted to 
work on these first.

It was found that the life puzzle had the second worst scaling performance so
David started working on it while Jack worked on matrix exponent. We decided
to implement both in OpenCL as there was a big window for scaling that could 
be improved.

From there, any person that finished one puzzle could move on to the next 
available one.

Performance analysis
===================================

The Xcode instruments tool showed the total time spent in all the function calls
in a program and also showed the most called functions. This helped us
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

    time(./bin/create_puzzle_input circuit_sim 1000 0 | ./bin/execute_puzzle 1 0  > /dev/null)

Design 
=======

While different methods were used to design the different programs, the 
general approach used to improve performance was to mainly look out for
opportunities of parallelism (i.e for loops, recursion) and redundant 
calculations. In the case of for loops, we would try to parallelize them 
depending on whether they had loop carried dependencies or not.

General Optimizations
=====================

Some optimizations that we did for all the OpenCL programs were generating
and caching compiled binaries of the kernels.

We also added logic to run either the reference version or our own version
of the puzzle functions depending on the scale as we observed that OpenCL
and TBB started to do well only from a certain scale due to startup costs.
We know that this is machine dependent picked some safe values based on 
extensive testing on various machines.

Life (David)
----

This was re-implemented in OpenCL. The `LifePuzzle::update()` function was 
exported into a kernel file.

The Optimizations done were the following:

- Changing the type of the state variable from a vector of `bool` to 
a vector of `uint8_t`. This is not only because we can only send primitive 
data types to the GPU but also due to the fact that vectors of bool are
special types of vectors that prevents us to create a pointer to their
first element, which is how we normally send buffers to the GPU.

Also , as only 1 bit is used in the flags, I used the smallest data size
possible for each which is uint8_t.

- Transferring and retrieving the state data only once to the GPU. As the
output state was always directly being fed back to the input, to reduce
communication cost, we copied the state into GPU buffers once and ran the kernel
there the number of times required after which the result would only be sent back

- In the update kernel, some of the destination co-ordinates ox and oy get
re-comptued unnecessarily so I made sure that these are calculated only once.

I recorded a speedup of 150+ for a scale of 5000 on a GTX Titan.

Matrix exponent (Jack)
---------------


String Search (Jack)
-------------

Circuit sim  (David)
-----------

This was implemented in TBB not only because it didn't scale as badly as
the others but also because it has a lot of branches (recursions) which I
don't think the GPU would've performed well at. 

I turned the recursive function calls inside `calcSrc()` into child tasks 
and set a bound on when the splitting should occur to limit the amount 
of task overhead. The bound is a dynamic number: it's a proportion of the 
scale (~60%). 

To determine if should split, I compare the bound with a number
which starts of as the scale and is subsequently divided by 2 in each 
recursive call. This number is meant to sort of represent the depth of the 
recursion tree.

Option explicit (David)
---------------

This was implemented in TBB. All the computational part was in the reference
execute function, no other functions. The improvements were the following:

- The `vU` and `vD` coefficients were recomputed several times, in total n^2 + n 
times in the entire program. So I computed them once and saved them in two 
buffers named `VUs` and `VDs`

- I added turned some loops into parallel_for where I could.


Median bits (JacK)
-----------


Conclusion
==========

Based on our results, we belive that the ideal solution would
have several implementations of the programs in different languages
and choose the most appropriate one based on the scale.

If time prevailed, we would've tried doing so by implementing all
the puzzle execute function in OpenCL and TBB.

It should be worth noting that in this entire project we put ourselves in
the shoes of real developers in industry and this influenced our decision 
making. (i.e  we took into account the cost of having specialised hardware
just to  run a program like median_bits for example for super large scales 
that will rarely be attempted)
