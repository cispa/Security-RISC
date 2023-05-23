CPU_NO_ATTACKER=0
CPU_NO_VICTIM=1

all: encrypt decrypt exploit exploit2 exploit-fr-parallel exploit-fr-sequential

encrypt: encrypt.c
	gcc -Lmbedtls/library -Imbedtls/include encrypt.c -ggdb3 -Wall -o encrypt -lmbedtls

decrypt: decrypt.c
	gcc -Lmbedtls/library -Imbedtls/include decrypt.c -ggdb3 -Wall -o decrypt -lmbedtls

decrypt-simulator: decrypt-simulator.c
	gcc -Lbp-simulator/library -Ibp-simulator/include decrypt-simulator.c -ggdb3 -Wall -o decrypt-simulator -lmbedtls

exploit: exploit.c simulator.c
	gcc -Lmbedtls/library -Imbedtls/include simulator.c exploit.c -ggdb3 -Wall -Os -o exploit -lmbedtls

exploit2: exploit2.c
	gcc -Lmbedtls/library -Imbedtls/include exploit2.c -ggdb3 -Wall -Os -o exploit2 -lmbedtls

exploit-fr-parallel: exploit_flush_reload_parallel.c
	gcc -Lmbedtls/library -Imbedtls/include exploit_flush_reload_parallel.c -ggdb3 -Wall -Os -o exploit-flush-reload-parallel -lmbedtls

exploit-fr-sequential: exploit_flush_reload_sequential.c
	gcc -Lmbedtls/library -Imbedtls/include exploit_flush_reload_sequential.c -ggdb3 -Wall -Os -o exploit-flush-reload-sequential -lmbedtls

run_encrypt: encrypt
	LD_LIBRARY_PATH=./mbedtls/library taskset -c ${CPU_NO_VICTIM} ./encrypt

run_decrypt: decrypt
	LD_LIBRARY_PATH=./mbedtls/library taskset -c ${CPU_NO_VICTIM} ./decrypt

run_exploit2: exploit2
	LD_LIBRARY_PATH=./mbedtls/library taskset -c ${CPU_NO_ATTACKER} ./exploit2

run_exploit: exploit
	LD_LIBRARY_PATH=./mbedtls/library taskset -c ${CPU_NO_ATTACKER} ./exploit

run_exploit_fr_par: exploit-fr-parallel
	#LD_LIBRARY_PATH=./mbedtls/library taskset -c ${CPU_NO_ATTACKER} ./exploit-flush-reload-parallel
	LD_LIBRARY_PATH=./mbedtls/library ./exploit-flush-reload-parallel

run_exploit_fr_seq: exploit-fr-seqential
	#LD_LIBRARY_PATH=./mbedtls/library taskset -c ${CPU_NO_ATTACKER} ./exploit-flush-reload-parallel
	LD_LIBRARY_PATH=./mbedtls/library ./exploit-flush-reload-sequential

clean:
	rm -f encrypt decrypt decrypt-simulator exploit2 exploit-flush-reload-sequential exploit-flush-reload-parallel
