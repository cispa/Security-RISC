# Code showing that the rdinstret instruction leaks instruction counts in M-Mode
We show that an attacker can observe the number of isntructions executed in the privileged M-Mode using the `rdinstret` instruction. 

# Run the code
Build the kernel module 
```
cd module 
make
sudo insmod sample.ko
```
Then build the `example.c` file with `make` and run it with `./example`

# Works on 
C906, U74