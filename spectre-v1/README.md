# Spectre v1

Simple demonstration of Spectre v1 style prefetching. 
A conditional branch is mistrained, leading to speculative fetching of code. 
The experiment shows that the code is sometimes cached even though it is not accessed architecturally. 
Adding several `nop`s before the transient execution of the victim function stops the leakage (poor man's fence). 

# Run Code
Run `make` then `./hist`

# Works on 
C906,U74 