# COMP428-Assignment3
Parallelized Floyd’s all-pairs shortest path algorithm using OpenMPI

For COMP 428 Assignment 3 Michael Bilinsky 26992358

To create all the executable files for this assignment, run the command "make assignment3". The files can be removed by calling "make clean"

First run "graph_generator" to generate an "input.txt" file with a graph to be processed. This file will be used by the following programs for sorting. The number of values can be specified as an argument to the program. For example the command "./graph_generator 1000" will create an "input.txt" file with 1 thousand nodes. Further parameters such as edge weight ranges and node disconnected rate are specified as constants in the program and can be modified from the source code.

The "serial_floyd" program can be executed with the "./serial_floyd" command to evaluate the input graph sequentially. The result will be written to "output.txt".

The "parallel_floyd_s1" and "parallel_floyd_s2" programs correspond to parallel implementations of Floyd’s algorithm using row and column broadcasts, and pipelining respectively. They can be executed with the "mpirun -np X parallel_floyd_sY" command where X is the number of processes to run the program on and Y is the number of the strategy to run. Note that the number of processes must be a square number in order to maintain the topology of a graph. The result will be written to "output.txt".