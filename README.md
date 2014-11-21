# CS441/541
##Project 4
---
**Author: Sanhu Li**

**Date: Nov. 21, 2014**

##Summary
---
This software is a memory management emulator. It simulates the process of handling pages in an operating system using serveral kinds of algorithms, including: OPT, FIFO, LRU, SC, ESC. The result represents the number of page faults in those conditions.

##Build
---
	$ make

If you have installed gcc properly, it will then automatically generate the software: pagefault

##Usage
---
	$ ./pagefault [-f num_of_frames] filename

Note that num_of_frames should between 1 and 7

##Example
---
```
$ ./pagefault input.txt -f 5
Num. Frames : 5
Ref. File   : input.txt
-------------------------------
Reference String:
 0:R,  1:W,  3:R,  6:R,  2:W,  4:R,  5:R,  2:W,  5:R,  0:R, 
 3:W,  1:R,  2:R,  5:W,  4:R,  1:R,  0:W
-------------------------------
####################################################
#     Frames    Opt.    FIFO     LRU      SC     ESC
#          5       8      12      12      13      12
####################################################
```
---
```
$ ./pagefault input.txt
Num. Frames : All
Ref. File   : input.txt
-------------------------------
Reference String:
 0:R,  1:W,  3:R,  6:R,  2:W,  4:R,  5:R,  2:W,  5:R,  0:R, 
 3:W,  1:R,  2:R,  5:W,  4:R,  1:R,  0:W
-------------------------------
####################################################
#     Frames    Opt.    FIFO     LRU      SC     ESC
#          1      17      17      17      17      17
#          2      13      16      16      16      15
#          3      11      15      15      15      15
#          4       9      14      14      14      15
#          5       8      12      12      13      12
#          6       7       9       9       9       9
#          7       7       7       7       7       7
####################################################
```

##How to test
---
You can enter `make check` to enable the default test scripts.

You can enter `make my-check` to enable the customed test scripts.

##About each test
---

##Know bugs and Problem areas
---
This software can only accept "regular" inputs.
Invaid inputs may cause unknown behaviours.

If you find any bugs, please report to <li.sanh@uwlax.edu>