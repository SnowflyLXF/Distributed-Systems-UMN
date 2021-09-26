CC = g++
CFLAGS = -g -Wall

all: Server client

Server: Servermain.cpp pbServers.cpp lwServers.cpp server.cpp
	$(CC) $(CFLAGS) $^ -o $@ 

client: client.cpp client.hpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	-rm -f *.o *~ *core* server client