output: build/main.o build/cctrl.o
	gcc -pthread build/main.o build/cctrl.o -o build/main

build/main.o: src/main.c
	gcc -c src/main.c -o build/main.o

build/cctrl.o: src/cctrl.c
	gcc -c -o build/cctrl.o src/cctrl.c

clean:
	rm build/*
