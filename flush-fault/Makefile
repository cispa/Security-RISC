all: flush-fault flush-ret

flush-fault: flush-fault.c
	gcc flush-fault.c -o flush-fault

flush-ret: flush-ret.c
	gcc flush-ret.c -o flush-ret


clean:
	rm -f flush-ret flush-fault *.csv
