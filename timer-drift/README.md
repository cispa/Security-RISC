# Timer Drift to Detect Network Activity

Look at the difference of retired instructions over a 1s interval to detect network activity. 
As the CPU frequency, and thus the cycles/second are fixed, and the retired instructions are counted for the core, this ratio show if something else, e.g., the kernel executes code. 
To test, run the program and from a different machine run `ping -s 50000 -i 3 <target machine ip>`.
