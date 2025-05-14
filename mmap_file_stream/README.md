# Joshua Moorehead fstream 2023

This project's use is to replace a subset of the fstream library. My goal was to correctly open and determine the size of a file, map that file to memory, be able to read from the file, and write/append to the file in memory. This project used memory mapping to achieve everything with the file.

## mmap_fstream.cc

- Includes the mmap_fstream.h
- Declares the class object and implements functions

## mmap_fstream.h

- includes
- Contains constructors (void, file name, and file name and mode) that construct the fstream by initiating its variables and calling open(). The call to open MEMORY MAPS THE FILE.
- Contains open(const std::string& filename) that opens the file using the std::open and maps it to memory based on fstreams permission flags AND truncates an entire page for it. Returns void.
- Contains open(const std::string& filename, std::ios_base::openmode mode) that opens the file using the std::open and maps it to memory after deciphering the correct mode permissions (entire page truncated too).  Returns void.
- Contains close() that closes the file descriptor, then truncates the file down to whatever size it ended up using, then SAVES IT TO DISC by using msync(). It also resets fd = -1 as there is no file open. It returns void.
- Contains is_open() that returns a boolean if the fstream is open or closed
- Contains size() that returns the current size of the file in size_t
- Contains get() that returns the "next" character of the file depending on where the index is. THIS READS FROM THE FILE.
- Contains put(char c) that writes a char to the file and updates the index. THIS WRITES TO THE FILE. It returns void.
- Contains private member variables size of file, index of file, const page size, file descriptor, filename, and address in memory.

## Makefile

- Use the command 'make' to compile and link the project
- Produces 'test_mmap_fstream' as executable
- Produces several test.txt files and test object files
- Use the command 'make clean' to remove all .o files
