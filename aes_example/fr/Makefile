lcryptpath = $(HOME)/openssl-1.0.1e/libcrypto.so
te0 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te0 | cut -d ' ' -f 1)
te1 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te1 | cut -d ' ' -f 1)
te2 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te2 | cut -d ' ' -f 1)
te3 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te3 | cut -d ' ' -f 1)


all: spy
clean:
	rm -f *.o spy
spy: spy.cpp 
	g++ -std=gnu++11 -DC906 -DLIBCRYPTO_PATH=\"$(lcryptpath)\" -DTE0=0x$(te0) -DTE1=0x$(te1) -DTE2=0x$(te2) -DTE3=0x$(te3) -O2 -o $@ $< -I~/openssl-1.0.1e/include/openssl -L~/openssl/ -lcrypto 

