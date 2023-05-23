all: hist-c906 hist-u74

hist-c906: hist.c
	gcc hist.c -DC906 -Os -o hist-c906

hist-u74: hist.c
	gcc hist.c -DU74 -Os -o hist-u74

clean:
	rm -f hist-c906 hist-u74

