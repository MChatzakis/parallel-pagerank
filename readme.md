# Pagerank: A Parallel Approach
Manos Chatzakis (csd4238@csd.uoc.gr)

### Compilation:
To compile the program you can use "make", supposing makefile and pagerank.c are in the same directory.

### Run:
To run the program, you may use the following pattern:
$ ./pagerank -f "your_dataset.txt"
And to measure the time:
$ time ./pagerank -f "your_dataset.txt"

### Selecting the threads:
You can change the "#define THREADS_NUM N" at the top of pagerank.c file.

### Supported Dataset Format:
The code can parse any dataset provided the follow the following format:
SrcID DstID
It ignores comment lines, which are the line that -START- with "#".

### Output File:
A csv file following the described format is producer in the same directory with the .c file, with name "pagerank.csv"

### Report - Measurements
A simple report describing the general development tactic is provided, separated in sections, where the method used is analyzed and the measurements are presented.