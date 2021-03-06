# Pagerank: A Parallel Approach
Manos Chatzakis (csd4238@csd.uoc.gr)

The folder is organized as follows:

pagerank:  
----datasets \
----metrics \
----source \
----report \
----readme.md

### Compilation:
To compile the program you can use "make", supposing makefile and pagerank.c are in the same directory.\
Make targets are: {all,gcc,clean}

### Run:
To run an instance of the program, use: \
$./page_rank -f input_dataset -m -t threads \
To see all possible configurations, type:\
$./page_rank -h

eg: \
$./page_rank -f datasets/enron.txt -m -t 2 \
will run the program with 2 threads for the enron dataset and it will print the total time for pagerank calculation.

### Supported Dataset Format:
The code can parse any dataset provided the follow the following format: \
SrcID DstID //no self loops allowed\
It ignores comment lines, which are the lines that start with "#".

### Output File:
A csv file following the described format is produced in the same directory with the executable as "pagerank.csv"

### Report:
A brief report presenting the development method and measurements is provided.
