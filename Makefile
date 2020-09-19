all: main

main:
	gcc main.c -o sample -lssl

clean:
	rm sample
