# AES T-table


# Run code
To compile opensssl 1.0.1e run the following commands in your **home** folder. 
```
wget https://ftp.openssl.org/source/old/1.0.1/openssl-1.0.1e.tar.gz
tar -xzf openssl-1.0.1e.tar.gz
cd openssl-1.0.1e
./config shared
make 
```

Then put the compiled openssl version in your library path
`export LD_LIBRARY_PATH=$HOME/openssl-1.0.1e:$LD_LIBRARY_PATH`

Run the PoC with the respective binary
`make; ./spy-c906` on the C906 processor or `make; ./spy-u74` on the U74 processor 


If the results dont look good you can 
1. Make sure `TE0-3` is in a decreasing order
2. change the threshold 
`#define MIN_CACHE_PROBE_CYCLES` and `#define MIN_CACHE_MISS_CYCLES`

# Works on
C906 (Flush+Reload, Prime+Probe, Evict+Reload), U74 (Prime+Probe, Evict+Reload)
