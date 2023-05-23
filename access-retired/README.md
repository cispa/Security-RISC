# Discover Hidden Files using Retired Instructions

This PoC detects if a file exists in a folder that does not allow a directory listing by relying on the retired-instructions performance counter. 
The PoC tries to open various files, which always fails (return value is NULL). 
However, the number of retired instructions is higher if the file exists. 

## Usage
Build the PoC using `make`, and create the inaccessible folder using `make prepare`. 
Then run `./main`. 

## Works on 
C906, U74