//Written by Joshua Moorehead 3/23 Copyright
#ifndef _INC_CLIENT_H_
#define _INC_CLIENT_H_
#include <sys/sysinfo.h>
#include "shared_memory.h"
#include "named_semaphore.h"
#include <vector>
#include <fstream>
#include <cmath>
#include <thread>
#include <mutex>


class Client{
 public:

  Client();

  //Destroys or "unlinks" shared memory

  void DestroyMemory();

  //Do forever while there are lines in the shared memory
  void RunClient(std::string msg, std::vector<std::string> searcht);

 private:
  static const std::size_t kBufferSize = 1024;
  static const std::size_t kSharedMemSize
    = SharedMemoryStoreSizeInPages(kBufferSize);

  const char* shm_name_;

  Shared_Memory_Store<kSharedMemSize> *store_;

  NamedSemaphore client_sem;
  NamedSemaphore server_sem;
  

  void ThreadProcess(std::vector<std::string>& v, size_t start, size_t end, std::vector<std::string>& searchTerms, std::vector<std::string>& printVector);
 

};

const std::vector<std::string> explode(const std::string& s, const char& c);
#endif  // _INC_CLIENT_H_