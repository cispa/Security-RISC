# TLB Evict Histogram
Prints a simple TLB eviction histogram. 
To evict, the PoC simply accesses 10 addresses on different pages forcing one address out of the TLB.

## Run code
Simply `make; ./hist` should produce an output simmilar to the one below. 

## Works on
C906, U74