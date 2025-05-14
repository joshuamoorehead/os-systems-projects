# Copyright 2023 Clay Crews

cc = g++
path = ..
standard = c++17

flags = -std=$(standard)
flags += -Wall
flags += -pedantic
flags += -I $(path)
flags += -g

link = $(cc) $(flags) -o 

compile = $(cc) $(flags) -c


all: text-client text-server
	
text-client: client.o
	$(link) text-client client.o
	
client.o: src/client.cc inc/client.h inc/UnixDomainSocket.h
	$(compile) src/client.cc

text-server: server.o
	$(link) text-server server.o

server.o: src/server.cc inc/server.h inc/UnixDomainSocket.h
	$(compile) src/server.cc


clean:
	$(RM) *.o text-client text-server
