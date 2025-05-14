// Copyright Joshua Moorehead CSCE311SP23

#ifndef INC_SERVER_H_
#define INC_SERVER_H_
#include <sys/sysinfo.h>
#include <string>
#include "../inc/shared_memory.h"
#include "../inc/named_semaphore.h"


class Server {
 public:
  Server();

  // function that runs forever
  int RunServer();

 private:
  static const std::size_t kBufferSize = 1024;
  static const std::size_t kSharedMemSize
    = SharedMemoryStoreSizeInPages(kBufferSize);

  std::string shm_name_;

  NamedSemaphore client_sem;  // shared memory semaphore
  NamedSemaphore server_sem;
};
#endif  // INC_SERVER_H_



