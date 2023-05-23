# Artifacts for A Security RISC
This repository contains the artifacts for the IEEE S&P 2023 paper "A Security RISC: Microarchitectural Attacks on Hardware RISC-V CPUs". You can find the paper at the [CISPA website](https://cispa.de/en/research/publications/3924-a-security-risc-microarchitectural-attacks-on-hardware-risc-v-cpus).

## Supported Processors and Distributions
Currently the experiments are tested on both the Alwinner C906 and SiFive U74 processors. 
You can find implementations specific to each of the two processors in the `./C906` and `./u74` subfolders. 
Software specifications from our paper:
- The C906 processors used run Debian 12 with with the kernels 5.14.0-rc4-nezha (Nezha) and 5.4.61 (Lichee RV).
- The U74 processors on the VisionFive board run Ubuntu 22.04.1 LTS with the 5.17.5-visionfive kernel 

## Utils
We provide a `riscsc.h` header file that bundles a few useful instructions for all experiments.
To use the header, simply add `#include "riscsc.h"` to your code, after copying the header to your path. 

## Materials
Each subfolder contains a README on how to run the specific experiment. We provide source code for the following experiments: 
### Case Studies
- `access-retired`: Discovers hidden files on a system by monitoring the ammount of retired instructions. This code is used during the Dropbox case study. 
- `zigzagger`: Code for the zigzagger case study showing that it is possible to distinguish the branch direction of zigzagger protected code via the count of retired instructions. 
- `aes_example`: Contains the code for all AES T-Table attacks performed. 
- `interrupt-timing`, `timer-drift`: Shows that it is possible to monitor network interrupts via timing. Also shows that a more precise result is possible using CycleDrift. 
- `page-walk`: Shows timing (and on the U74 instruction differences) that allow to distinguish the length of the page-table walk. 
- `mbedtls-key-leak`: Contains the code for the Flush+Fault exploit on MbedTLS.

### Histogram Code 
- `evict_reload_histrogram`: Generates a histrogram for an Evict+Reload covert channel 
- `fence_flush_histogram`: Generates a histrogram for an Flush+Reload attack on the I-Cache using the `fence.i` instruction.  
- `fgprime_probe_histrogram`: Optimized Prime+Probe for the C906 making use of the FIFO relacement strategy to achive highter transfer rates. 
- `iflush_reload_histogram`: Generates a Flush+Reload histogram using the `icache.iva` instruction on the C906. 
- `flush_reload_histogram`: Generates a Flush+Reload histogram using the `dcache.iva` instruction on the C906. 
- `flush_flush_histogram`: Generates a Flush+Flush histogram using the `dcache.iva` instruction on the C906. 
- `prime_probe_histogram`: Histrogram for a Prime+Probe attack. 
- `tlb_evict_histogram`: Code to genereate a histogram showing that it is possible to evict TLB entries. 
- `spectre-v1`: Histogram that shows that code is spculatively fetched, enabling our Cache+Time attack. 

### Benchmarks
- `timer-evaluation`: Benchmark for the precision and increment behavior of the timers present on the tested processors. 
-  `instr-cycles`: Shows the timing of most base set instructions  

### Misc
- `square-multiply`: Padded square and multiply implementation that is still vulnerable to an attacker that can see the number of retired instructions. 
- `m-mode-instr-count`: Shows that the `rdinstret` instruction leaks the number of exeuted instructions in M-mode. 
- `flush-fault`: PoC implementations for both variants of Fault+Fault.

## Citing Paper and Artifcats
If you use our results in your research, please cite our paper as:
```
@inproceedings{Gerlach2023SecurityRISC,
 author = {Gerlach, Lukas and Weber, Daniel and Zhang, Ruiyi and Schwarz, Michael},
 booktitle = {IEEE S&P},
 title = {{A Security RISC: Microarchitectural Attacks on Hardware RISC-V CPUs}},
 year = {2023}
}
```
And our artifacts as:
```
@misc{Gerlach2023SecurityRISCartifacts,
 author = {Gerlach, Lukas and Weber, Daniel and Zhang, Ruiyi and Schwarz, Michael},
 url = {https://github.com/cispa/Security-RISC}
 title = {{A Security RISC: Microarchitectural Attacks on Hardware RISC-V CPUs Artifact Repository}},
 year = {2021}
}
```
