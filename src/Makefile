INCLUDE=-I./lib/http_connector -I./
LIBS=-L./lib/http_connector -ljson-c -lhttp_connector -lssl

all: main

main:
	gcc main.c openweather.c -o sample $(LIBS)

clean:
	rm sample
