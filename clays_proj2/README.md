# Clay Crews - CSCE 311 - Project 2

## server.cc

- Contains main() for the text-server executable, which calls SocketServer.RunServer() to start the server with command line arguments
- Contains the class SocketServer, which inherits from the UnixDomainSocket class
	- SocketServer class has public function RunServer(), which takes in the command line arguments, int argc and char *argv[]
	- RunServer() builds a Unix Domain Socket using the socket name provided by the user in the command line
	- RunServer() begins listening for client connections at a maximum of the number of executions contexts on the machine - 1
	- RunServer() accepts client connections
	- For each client connection, RunServer() reads n strings from the client including file path, search terms, and search operators
	- For each client connection, RunServer() uses the path to open the file and check for an invalid file
	- For each client connection, RunServer() parses the file at path and writes any line containing the search expression to the client
	- For each client connection, RunServer() prints the number of bytes sent to the client
	- RunServer() stays alive waiting for a new client connection
- Contains the helper function split_str()
- Contains the helper fucntion writeCheck()

## server.h

- Includes supporting files
- Function definition for split_str()
- Function definition for writeCheck()

## client.cc

- Contains main() for the text-client executable, which calls SocketClient.RunClient() to start the server with command line arguments
- The class SocketClient inherits from the UnixDomainSocket class
	- SocketClient class has public function RunClient(), which takes in the command line arguments, int argc and char *argv[]
	- RunClient() connects to an existing server using the command line argument 
	- RunClient() sends the file path to the server
	- RunClient() sends the search expression to the server
	- RunClient() reads from the server and prints each line 
	- RunClient() prints the number of bytes recieved from server

## client.h

- Includes supporting files

## UnixDomainSocket.h

- Creates a Unix Domain Socket with the given socket_path

## makefile

- Use the command 'make' to make text-server and text-client executables
- Use the command 'make text-server' to compile and link the text-server
- Produces 'text-server' as an executable
- Use the command 'make text-client' to compile and link the text-client
- Produces 'text-client' as an executable
- Use the command 'make clean' to remove all intermediary and executable files


