#Copyright Joshua Moorehead 3/23

cc = g++ 
path = ..
standard = c++17
#TARGET = calculate

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation

link = $(cc) $(flags) -o  # final linked build to binary executable
compile =  $(cc) $(flags) -c # compilation to intermediary .o files

all: text-client text-server

text-client: domain_socket_client.o
	$(link) text-client domain_socket_client.o

text-server: domain_socket_server.o
	$(link) text-server domain_socket_server.o

#OBJECTS

domain_socket_client.o: src/domain_socket_client.cc inc/domain_socket_client.h inc/sock.h
	$(compile) src/domain_socket_client.cc

domain_socket_server.o: src/domain_socket_server.cc inc/domain_socket_server.h inc/sock.h
	$(compile) src/domain_socket_server.cc
clean:
	$(RM) *.o text-client text-server
