# mbedTLS Flush+Fault

# Install Dependencies (local machine)
```bash
apt-get install python3 python3-pip python3-dev git libssl-dev libffi-dev build-essential
pip --user install pandas pwntools
```
As installing these dependencies on the RISC-V systems can be tricky, we assume that you use another machine (later on referred to as "local") which is running a commodity Linux.

# Run code
To execute the experiment run the following commands.
Please note that commands starting with "lab-machine>" must be executed on the RISC-V system and commands starting with "local>" must be executed on the previously mentioned second machine.

```bash
# build initial binary
lab-machine> python3 build.py

# copy binary to local machine to generate offsets
local> scp lab-machine:<FOLDER>/mbedtls-key-leakage/mbedtls/library/libmbedtls.so.8 .
local> python3 ./generate_header.py

# copy header back to lab machine and build again
local> scp ./attack_offsets.h lab-machine:~/mbedtls-key-leakage/ 
lab-machine> python3 build.py

# get the attack traces
lab-machine> sudo python3 ./evaluate-attack.py get-leakage

# postprocess them locally to read the key
local> scp lab-machine:~/risc-attack-eval risc-attack-eval/
local> python3 ./evaluate-attack post-process <THRESHOLD>
```

## Choosing the right threshold
The last command from the previous section can be given an threshold, which is the number of cycles that distinguish 0-bits from 1-bits. 
This threshold has to be adjusted depending on your system configuration and resulting execution times.
The post-processing script outputs the medians of all 0- and 1-bits which should give a good estimate for the threshold.

# Works on
C906 , U74
