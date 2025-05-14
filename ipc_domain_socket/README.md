# Joshua Moorehead Client Server Copyright Feb 2023

This project's use is to demonstrate IPC between a server and multiple clients. The server and clients use several different Unix Domain Sockets to communicate. The objective of the communication is to find certain strings in a given csv file based on search commands from the command line. Several files detailed below facilitate the communication and sorting of csv data. 

## domain_socket_server.cc

- Includes the domain_socket_server.h 
- Contains a main() - that initiates a server based on the second command line argument, then runs the server. 
- RunServer() builds a socket, listens for clients, accepts clients, and for each client it has a while loop that reads n strings through a buffer and the read function, parses file and finds matching lines, returns lines to client

## domain_socket_server.h

- includes "sock.h" for a the UnixDomainSocket class that has various socket variables needed 
- includes <sys/sysinfo.h> for finding nprocesses on machine
- includes <vector> for dividing client message using explode function 
- Contains DomainSocketServer class that inherits from UnixDomainSocket
- Contains a RunServer() in DomainSocketServer that does all the tasks we need the server to do, such as connect to clients, read their messages, parse the messages, find the correct lines from csv file, and return it. 
- Contains explode() function from c++ documentation that parses a string by given delimiter

## domain_socket_client.cc

- Includes "domain_socket_client.h", that 
- Contains a main() - that initiates a client based on the second command line argument, then runs the client. 
- RunClient() 


## domain_socket_client.h

- includes "sock.h" for a the UnixDomainSocket class that has various socket variables needed 
- includes <sys/sysinfo.h> 
- Contains DomainSocketServer class that inherits from UnixDomainSocket
- Contains a RunClient() in DomainSocketServer that does all the tasks we need the client to do, such as connect to server, send their messages, parse the messages, find the correct lines from csv file, and return it. 
- Contains a parseCommandLine() that takes in the arguments and separates it to file name, op code, and all remaining string arguments
- 


## sock.h

- Creates a Unix Domain Socket with various variables such as sock memory address and its path
- pulled from Lewis' github

## Makefile

- Use the command 'make' to compile and link the project
- Produces 'text-client' and 'text-server' as executables 
- Use the command 'make clean' to remove all .o files
