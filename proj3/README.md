# Joshua Moorehead Project3 Client Server Shared Memory IPC CSCE311-001

This project's use is to demonstrate IPC with shared memory between a server and a client. The server and clients use the same shared memory to communicate. The objective of the communication is to pass a csv file and search it for matching output to stdlog. Several files detailed below facilitate the communication and sorting of csv data. This project's main concepts included shared memory and semaphores (named) between processes, and multithreading. 

## server.cc

- Includes the server.h 
- Contains a main() - that initiates a server's semaphores and shared memory names (hardcoded) and creates the semaphores. 
- RunServer() that blocks itself. Once it receives a signal from client it connects to shared memory, reads the file path, and puts it's contents into shared memory using an fstat method I found online. It closes shared memory and destorys semaphores after that. 

## server.h
 
- includes <sys/sysinfo.h> for finding nprocesses on machine
- - includes "shared_memory.h" for shared memory struct when tapping into shared memory
- includes "named_semaphore.h" for semaphore abilities 
- Contains Server class
- Contains a RunServer()  
- Contains various memory variables, shared memory name, and semaphores

## client.cc

- Includes "client.h"
- Contains a main() - that initiates a client, its variables include memory sizes, buffer sizes, 2 semaphores, and memory names/addresses, based on the second command line argument, then runs the client. 
- RunClient() first sends the file name to server via the shared memory it created when initiliazed, then it signals the server via shared named semaphore, then it blocks itself. Once it receives a signal that the file is in shared memory, it stores it into a string. It uses the explode function to put the string contents into a vector. It unmmaps memory. Then created 4 threads that all search the vector contents in the ThreadProcess() function. They store matching lines in a new vector that then gets printed back in RunClient(). 


## client.h

- includes <sys/sysinfo.h> 
- includes "shared_memory.h" for shared memory struct when tapping into shared memory
- includes "named_semaphore.h" for semaphore abilities 

- Contains a RunClient() in main that does all the tasks we need the client to do, such as connect to server, send their messages, parse the messages, find the correct lines from csv file, and return it. 
- Contains a parseCommandLine() that takes in the arguments and separates it to file name, op code, and all remaining string arguments
- Contains a DestoryMemory() that unmmaps the shared memory
- Contains an "explode" function that puts strings into vectors based off a delimiter
- Contains ThreadProcess() that takes in multiple arguments to search a vector for matching lines

## named_semaphore.cc

- Includes "../inc/named_semaphore.h"
- Contains Create(int count) - that creates a semaphore by using sem_open()
- Contains Open() that opens a semaphore using the same sem_open but not the create version, only the read version. 


## named_semaphore.h

- includes <sys/sysinfo.h> 
- include <fcntl.h> - POSIX open flags (O_CREAT, O_RD, O_RDWR, O_WR)
- include <semaphore.h>  - POSIX semaphore support
- include <sys/stat.h>  for POSIX support

- Contains a Destory() that unlinks a semaphore.
- Contains a Down() - that uses sem_wait() to wait for a signal
- Contains a Up() - that uses sem_post() to send a signal
- Contains a name and sem_t* for variables

- 

## shared_memory.h

- include <sys/mman.h>  // POSIX shared memory map
- include <sys/unistd.h>  // Unix standard header (ftruncate)
- include <unistd.h>  // Unix standard header, using close
- includes <sys/sysinfo.h> 
- Contains a struct shared memory for a buffer size and char array. This is how the bytes get actually stored in shared memory
- contains a function to calculate page size needed
- 
## Makefile

- Use the command 'make' to compile and link the project
- Produces 'csv-client' and 'csv-server' as executables 
- Use the command 'make clean' to remove all .o files