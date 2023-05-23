# Prime+Probe Histogram

Prints a simple Prime+Probe histogram. 
To prime a cache set, the PoC simply accesses 6 addresses that (probably) fall into the same set of the 4-way set-associative data cache. 

# Run
Running `make` gives two binaries, run `hist-c906` if testing on the C906 processor and `hist-u74` if testing on the U74 processor. 

# Works on 
C906, U74