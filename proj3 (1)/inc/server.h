
#include <sys/sysinfo.h>
#include "shared_memory.h"
#include "named_semaphore.h"


class Server {
 public:

  Server();

  ~Server(); // destroys all named semaphores
  

  //function that runs forever
  int RunServer() ;

 private:

  static const std::size_t kBufferSize = 1024;
  static const std::size_t kSharedMemSize
    = SharedMemoryStoreSizeInPages(kBufferSize);

  std::string shm_name_;

  NamedSemaphore client_sem;//shared memory semaphore
  NamedSemaphore server_sem;


  

};



