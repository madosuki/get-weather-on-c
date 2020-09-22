LIBS=-lssl

all: static

shared:
	gcc -fPIC -c ./http_connector.c -o ./http_connector.share.o $(LIBS)
	gcc -shared -Wl,-soname,libhttp_connector.so -o libhttp_connector.so.1.0 ./*.share.o -lc
	ldconfig -n ./

static:
	gcc -c ./http_connector.c -o ./http_connector.static.o $(LIBS)
	ar rcs libhttp_connector.a ./http_connector.static.o

clean:
	rm ./*.o ; rm ./*.so* ; rm ./*.a
