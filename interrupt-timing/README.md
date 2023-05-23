# Interrupt timing exeriments
These experiments show that an attacks is able to monitor network interrupts send to a machine. 

# How to run
Running `make; ./attacker` starts the attacker process which monitors for timing differences in execution induced by network interrupts. 
To test that the attacker works you will have to send network interrupts to the machine. 
To test, run the program and from a different machine run `ping -s 50000 -i 3 <target machine ip>`.

# Works on 
C906,U74
