output: build/main.o build/cctrl.o
	gcc -pthread build/main.o build/cctrl.o -o build/main

build/main.o: src/main.c build
	gcc -c src/main.c -o build/main.o

build/cctrl.o: src/cctrl.c build
	gcc -c -o build/cctrl.o src/cctrl.c

built:
	mkdir -p built

clean:
	rm build/*
