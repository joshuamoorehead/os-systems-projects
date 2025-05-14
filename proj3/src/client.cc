// Written by Joshua Moorehead 3/23 Copyright
#include "../inc/client.h"
#define SNAME "/clientsem"
#define SNAME2 "/serversem"
#define shm_name "shm"


const char kUS = static_cast<char>(31);
const char kEoT = static_cast<char>(3);
const char kEoL = static_cast<char>(10);

pthread_mutex_t mtx;

struct ThreadParams {
    std::vector<std::string>& v;
    int start_index;
    int end_index;
    std::vector<std::string>& OpsearchTerms;
    std::vector<std::string>& printVector;
};

Client::Client()
    : shm_name_(shm_name),
      client_sem(SNAME) , server_sem(SNAME2) {
  // (1) Create shared memory location with struct
  // open shared memory and capture file descriptor
  int shm_fd = ::shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0660);
  // Open signal mux (unlocked by producer(s))

  client_sem.Open();
  server_sem.Open();
  if (shm_fd < 0) {
    DestroyMemory();
    std::cerr <<
    "Consumer::Consumer --- ::shm_open(CLIENT CAN'T OPEN SHARED MEMORY): "
     << shm_name_ << ") --- "
      <<  ::strerror(errno) << std::endl;
    ::exit(errno);
}
// set size of shared memory with file descriptor
  if (::ftruncate(shm_fd, kSharedMemSize) < 0) {
    DestroyMemory();
    std::cerr << ::strerror(errno) << std::endl;

    ::exit(errno);
  }

  // get copy of mapped mem
  const int kProt = PROT_READ | PROT_WRITE;
  store_ = static_cast
  <Shared_Memory_Store<kSharedMemSize>*>(
  // Essentially subbing in 4096 for buffer size
    ::mmap(NULL, kSharedMemSize, kProt, MAP_SHARED, shm_fd, 0));

  if (store_ == MAP_FAILED) {
    DestroyMemory();
    std::cerr << "MAPPING FAILED" << ::strerror(errno) << std::endl;

    ::exit(errno);
  }

  // init memory map
  store_->buffer_size = kBufferSize;  // set store's buffer size
}




void Client::DestroyMemory() {
  // return copy of mapped mem, capture any error/exit code
  int exit_code = ::munmap(store_, kSharedMemSize);
  // alert for error in ::munmap
  if (exit_code < 0) {
    std::cerr << ::strerror(errno) << std::endl;
  }


  // delete shared memory map
  if (::shm_unlink(shm_name_) < 0) {
    std::cerr << ::strerror(errno) << std::endl;
  }

  ::exit(errno);
}



void* Client::ThreadProcess(void* params) {
  ThreadParams* thread_params = static_cast<ThreadParams*>(params);
  std::vector<std::string>& v = thread_params->v;
  size_t start = thread_params->start_index;
  size_t end = thread_params->end_index;
  std::vector<std::string>& OpsearchTerms = thread_params->OpsearchTerms;
  std::vector<std::string>& printVector = thread_params->printVector;

  std::string opCode = OpsearchTerms[0];
  int count(0);

  for (size_t i = start; i < end; ++i) {
    std::string line = v[i];
    bool matchingLine = false;

    if (line.compare("INVALID FILE") == 0) {  // client deemed it invalid
         std::cerr << "INVALID FILE" << std::endl;
         exit(-1);
    }
    if (opCode.compare("n/a") == 0) {
      if (line.find(OpsearchTerms[1]) != std::string::npos)
        matchingLine = true;
    } else if (opCode.compare("OR") == 0) {
        for (size_t j = 1; j < OpsearchTerms.size(); ++j) {
        if (line.find(OpsearchTerms[j]) != std::string::npos) {
          matchingLine = true;
          break;
        }
      }
    } else if (opCode.compare("AND") == 0) {
      for (size_t j = 1; j < OpsearchTerms.size()-1; ++j) {
        if (line.find(OpsearchTerms[j]) != std::string::npos) {
          matchingLine = true;
        }
        if (line.find(OpsearchTerms[j]) == std::string::npos) {
          matchingLine = false;
          break;
        }
      }
    }
    if (matchingLine) {
      pthread_mutex_lock(&mtx);
      printVector.push_back(line);
      pthread_mutex_unlock(&mtx);
    }
  }
  delete params;
  pthread_exit(NULL);
}


void Client::RunClient(std::string file,
 std::vector<std::string> searchterms)  {
  // (2) Send file name to server via shared memory
  strncpy(store_->buffer, file.c_str(), store_->buffer_size);
  server_sem.Up();  // After sending file path, signals server

  client_sem.Down();

  // (3) Copy file contents into "local storage" aka vector
  std::string file_data = store_->buffer;
  std::vector v = explode(file_data, kEoL);
  // puts data string into vector based on delim value, pulled from c++ website


  // return copy of mapped mem, capture any error/exit code
  int exit_code = ::munmap(store_, kSharedMemSize);
  // alert for error in ::munmap
  if (exit_code < 0) {
    std::cerr << ::strerror(errno) << std::endl;
  }


  // delete shared memory map
  if (::shm_unlink(shm_name_) < 0) {
    std::cerr << ::strerror(errno) << std::endl;
  }

  // (4) Threads the searching vector process. Each thread gets 1/4 the lines
  size_t step = std::ceil(static_cast<double>(v.size())/4);
  size_t start_index(0);
  size_t end_index(step);
  // creating 4 threads
  pthread_t threads[4];
  std::vector<std::string> printVector;

  for (int i =0; i < 4; i++) {
    ThreadParams* params = new ThreadParams{v, start_index, end_index, searchterms, printVector}; //NOLINT
    pthread_create(&threads[i], NULL, ThreadProcess, (void*) params);//NOLINT
    start_index = end_index;
    end_index = start_index + step;
  }

  // join the pthreads
  for (int i = 0; i < 4; i++) {
      pthread_join(threads[i], NULL);
  }


  // (5) Writes results to STDOUT
  for (size_t i = 0; i < printVector.size(); ++i) {
    std::cout << i+1 << "\t" << printVector[i] << std::endl;
  }

  ::exit(errno);
}



std::string parseCommandLine(int argc, char** argv) {
  std::string message = "";
  int counter(0);
  int checkerOr(0);
  int checkerAnd(0);

  for (int i = 2; i < argc; i++) {  // checking for mixed op code
    if (strcmp(argv[i], "+") == 0) {
      checkerOr++;
    }
    if (strcmp(argv[i], "x") == 0) {
      checkerAnd++;
    }

    if ((checkerOr > 0) && (checkerAnd > 0)) {
      std::cerr
      << "Mixed boolean operations not presently supported"
      << std::endl;
      exit(-1);
    }
  }

  for (int i = 3; i < argc; i++) {  // adding all the opcode now to the message
    if (((strcmp(argv[i], "+") == 0 )
    || strcmp(argv[i], "x") == 0) && counter == 0) {
      if (strcmp(argv[i], "+") == 0) {
        message.append("OR");
        counter++;
        message += (kUS);
      }
      if (strcmp(argv[i], "x") == 0) {
        message.append("AND");
        counter++;
        message += (kUS);
      }
    }
  }
  if (counter == 0) {
    message.append("n/a");
    counter++;
    message += (kUS);
  }
  for (int i= 2; i < argc; i+=2) {
    if (strcmp(argv[i], "+") != 0 || strcmp(argv[i], "x") != 0) {
      message.append(argv[i]);
      message += (kUS);
    }
  }
  message += (kEoT);
  return message;
}




const std::vector<std::string> explode(const std::string& s, const char& c) {
  std::string buff{""};
  std::vector<std::string> v;

  for (auto n : s) {
    if (n != c) buff+=n; else //NOLINT
    if (n == c && buff != "") { v.push_back(buff); buff = ""; } //NOLINT
  }
  if (buff != "") v.push_back(buff);

  return v;
  // the first index of vector is file name, then op code, then arg strings
}

int main(int argc, char** argv) {  // takes in command line arguments
  // parse command line args and store in a string
  std::string opcodeandsearchterms = parseCommandLine(argc, argv);
  std::vector searchterms = explode(opcodeandsearchterms, kUS);

  Client client;

  client.RunClient(argv[1], searchterms);

  return 0;
}


