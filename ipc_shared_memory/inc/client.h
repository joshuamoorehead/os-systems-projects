// Written by Joshua Moorehead 3/23 Copyright

#ifndef INC_CLIENT_H_
#define INC_CLIENT_H_
#include <sys/sysinfo.h>
#include <pthread.h>
#include <vector>
#include <fstream>
#include <cmath>
#include <string>
#include <thread> //NOLINT
#include <mutex> //NOLINT

#include "../inc/shared_memory.h"
#include "../inc/named_semaphore.h"



class Client{
 public:
  Client();

  // Destroys or "unlinks" shared memory

  void DestroyMemory();

  // Do forever while there are lines in the shared memory
  void RunClient(std::string msg, std::vector<std::string> searcht);

 private:
  static const std::size_t kBufferSize = 1024;
  static const std::size_t kSharedMemSize
    = SharedMemoryStoreSizeInPages(kBufferSize);

  const char* shm_name_;

  Shared_Memory_Store<kSharedMemSize> *store_;

  NamedSemaphore client_sem;
  NamedSemaphore server_sem;

  static void* ThreadProcess(void* params);
};

const std::vector<std::string> explode(const std::string& s, const char& c);
#endif  // INC_CLIENT_H_
