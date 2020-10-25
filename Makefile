CC=gcc
CFLAGS=-Wall -Wextra

OBJS=Server.o Net.o File.o Mime.o Cache.o TablaHash.o ListaEnlazada.o Queue.o

all: Server

Server: $(OBJS)
	gcc -pthread -g -o $@ $^

Net.o: Net.c Net.h

Server.o: Server.c

File.o: File.c File.h

Mime.o: Mime.c Mime.h

Cache.o: Cache.c Cache.h

TablaHash.o: TablaHash.c TablaHash.h

ListaEnlazada.o: ListaEnlazada.c ListaEnlazada.h

Queue.o: Queue.c Queue.h

clean:
	rm -f $(OBJS)
	#rm -f Server
	#rm -f cache_tests/cache_tests
	#rm -f cache_tests/cache_tests.exe
	#rm -f cache_tests/cache_tests.log
