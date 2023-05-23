# Instruction Flush+Reload Histogram

Prints a simple Flush+Reload histogram for the instruction cache. 
To flush, the PoC uses the custom C906 instruction `icache.iva` which flushes a virtual address from the instruction cache. 

# Run Code
Run `make` then `./hist`

# Works on 
C906 (because custom cache flush instruction)