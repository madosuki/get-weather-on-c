INCLUDE=-I./lib/http_connector
LIBS=-L./lib/http_connector -ljson-c -lhttp_connector -lssl

all: main

main:
	gcc main.c -o sample $(LIBS)

clean:
	rm sample
