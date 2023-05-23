lcryptpath = $(HOME)/openssl-1.0.1e/libcrypto.so
te0 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te0 | cut -d ' ' -f 1)
te1 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te1 | cut -d ' ' -f 1)
te2 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te2 | cut -d ' ' -f 1)
te3 := $(shell nm ~/openssl-1.0.1e/libcrypto.so | grep Te3 | cut -d ' ' -f 1)

all: spy-u74 spy-c906

clean:
	rm -f *.o spy-u74 spy-c906

spy-c906: spy.cpp
	g++ -g -std=gnu++11 -DC906 -DLIBCRYPTO_PATH=\"$(lcryptpath)\" -DTE0=0x$(te0) -DTE1=0x$(te1) -DTE2=0x$(te2) -DTE3=0x$(te3) -Os -o $@ $< -I~/openssl-1.0.1e/include/openssl -L~/openssl/ -lcrypto
	
spy-u74: spy.cpp 
	g++ -g -std=gnu++11 -DU74 -DLIBCRYPTO_PATH=\"$(lcryptpath)\" -DTE0=0x$(te0) -DTE1=0x$(te1) -DTE2=0x$(te2) -DTE3=0x$(te3) -Os -o $@ $< -I~/openssl-1.0.1e/include/openssl -L~/openssl/ -lcrypto
