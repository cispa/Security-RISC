# Flush+Reload Histogram

Prints a simple Flush+Reload histogram. 
To flush, the PoC uses the custom C906 instruction `dcache.civa` which flushes a virtual address from the data cache. 

# Run Code
Run `make` then `./hist`

# Works on 
C906 (because custom cache flush instruction)