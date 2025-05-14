// Copyright Joshua Moorehead March2023

#include "../inc/server.h"


#define SNAME "/clientsem"
#define SNAME2 "/serversem"
#define shm_name "shm"


Server::Server() : shm_name_(shm_name), client_sem(SNAME), server_sem(SNAME2) {
  // get shared memory log signal named semaphore
  //   created by and locked by consumer
  ::sem_unlink(SNAME);
  ::sem_unlink(SNAME2);
  shm_name_ = shm_name;
  client_sem.Create(0);
  server_sem.Create(0);
  client_sem.Open();
  server_sem.Open();
}


int Server::RunServer() {
  const char kUS = static_cast<char>(31);
  const char kEoT = static_cast<char>(3);

  // Blocked on semaphore
  server_sem.Down();


  // open shared memory with read/write rights
  // and get file descriptor after client signals
  int shm_fd = ::shm_open(shm_name, O_RDWR, 0);
  if (shm_fd < 0) {
    std::cerr <<
    "Server::Server ::shm_open: SERVER SM OPEN FAILED" << ::strerror(errno)
      << std::endl;
    return errno;
  }

  // get copy of mapped mem
  Shared_Memory_Store<kSharedMemSize>* store
    = static_cast<Shared_Memory_Store<kSharedMemSize>*>(
      ::mmap(NULL,
             kSharedMemSize,
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             shm_fd,
             0));
    if (store == MAP_FAILED) {
    std::cerr << "Server::Server ::mmap: COPY OF MMAP FAILED"
    << ::strerror(errno)
      << std::endl;
    return errno;
    }

  // Attempt to acquire the locked barrier semaphore
  std::string fileName = store->buffer;
  // (2)
  std::clog << "CLIENT REQUEST RECEIVED" << std::endl;

  // (3) Memory is already opened from server initialization
  std::clog << "\tMEMORY OPEN" << std::endl;

  // (4) Using the path open csv file and write to shared memory

  std::clog << "\tOPENING: " << fileName << std::endl;

  int csv_fd = open(fileName.c_str(), 00, S_IRUSR | S_IWUSR);
  if (!csv_fd) {
     std::string invalid = "INVALID FILE";
     invalid += static_cast<char>(10);
      // Write back to client using shared memory
      strncpy(store->buffer, invalid.c_str(), store->buffer_size);
      close(shm_fd);
      // unlink semaphores
      client_sem.Destroy();
      server_sem.Destroy();
  }
  struct stat csv_size;
  fstat(csv_fd, &csv_size);
  char* csv_in_memory = static_cast<char*>(::mmap(nullptr,
  csv_size.st_size, PROT_READ, MAP_PRIVATE, csv_fd, 0));
  strncpy(store->buffer, csv_in_memory, csv_size.st_size);
  close(csv_fd);  // close file

  std::clog << "\tFILE CLOSED: " << fileName << std::endl;
  int exit = munmap(csv_in_memory, csv_size.st_size);
  if (exit < 0) {
    std::cerr << "Unmap error: " << errno << std::endl;
  }
  client_sem.Up();  // signal client that file has been put into shared memory
  // unlink from shared memory
  close(shm_fd);
  std::clog << "\tMEMORY CLOSED!" << std::endl;
  // unlink semaphores
  client_sem.Destroy();
  server_sem.Destroy();
}

int main(int argc, char** argv) {
  while (true) {
  Server server;
  std::clog << "SERVER STARTED" << std::endl;

  server.RunServer();
  }
  return 0;
}

