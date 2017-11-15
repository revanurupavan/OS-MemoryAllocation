Purpose of the project:

The purpose of the project is to work with memory management system and the algorithms (First fit, Best fit and Worst fit) linux using pthreads.



Name of the Source Code files:
1. MMS.c

How to run the executable files:
1 ./MMS.o 4 1 for First fit
2 ./MMS.o 4 2 for Best fit
3 ./MMS.o 4 3 for Worst fit


Note: The program is made to run with 5 memory block partitions created by malloc().

If there are more than 5 threads created, then there won't be any block available for the rest of the threads and results in a segmentation fault.

