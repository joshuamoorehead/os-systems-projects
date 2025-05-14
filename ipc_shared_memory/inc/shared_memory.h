#ifndef INC_SHARED_MEMORY_H_
#define INC_SHARED_MEMORY_H_


#include <sys/mman.h>  // POSIX shared memory map
#include <sys/unistd.h>  // Unix standard header (ftruncate)
#include <unistd.h>  // Unix standard header, using close

#include <cassert>  // using assert
#include <cerrno>  // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror

#include <string>
#include <iostream>


template <std::size_t BufferSize>
struct Shared_Memory_Store {
    std::size_t buffer_size;
    char buffer[BufferSize];
    // char buffer[1];
    /* data */
};

constexpr std::size_t SharedMemoryStoreSizeInPages(
    std::size_t buffer_size, std::size_t page_size = 4096) {
  return page_size * (1 + (buffer_size + sizeof(std::size_t)) / page_size);
}


#endif  // INC_SHARED_MEMORY_H_
