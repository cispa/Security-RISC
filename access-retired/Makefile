all: 
	gcc main.c -Os -o main
	
prepare:
	mkdir sec
	touch sec/foo
	chmod 0000 sec/foo
	chmod 0111 sec
clean:
	rm -f main
	sudo rm -rf sec

