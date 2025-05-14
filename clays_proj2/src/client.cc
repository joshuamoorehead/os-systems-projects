// Copyright 2023 Clay Crews

#include "../inc/client.h"

class SocketClient : public UnixDomainSocket {
 public:
  using ::UnixDomainSocket::UnixDomainSocket;

  ssize_t writeCheck(ssize_t written) {
    if (written < 0) {
      std::cerr << strerror(errno) << std::endl;

      exit(-1);
    }

    if (written == 0) {
      std::clog << "Server dropped connection!" << std::endl;
      exit(-2);
    }

    return written;
  }

  void RunClient(int argc, char *argv[]) const {
    // open socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // connect to existing socket
    int success = connect(socket_fd,
                          reinterpret_cast<const sockaddr*>(&sock_addr_),
                          sizeof(sock_addr_));

    if (success < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // connected to socket
    std::clog << "SERVER CONNECTION ACCEPTED" << std::endl;

    const char kUS = static_cast<char>(31);
    const char kEoT = static_cast<char>(3);

    // parse client data message
    //  "file_name<US>op_code<US>search_string_1<US>search_string_2<US>...<US>search_string_n<EOT>"
    std::string msg;
    msg += argv[2];
    msg += kUS;

    // define op_code and search_strings
    std::string op_code = "n/a";
    std::string search_strings;
    search_strings += kUS;
    search_strings += argv[3];

    if (argc > 4) {
      if (strcmp(argv[4], "+") == 0) {
        op_code = "OR";
      } else if (strcmp(argv[4], "x") == 0) {
        op_code = "AND";
      }

      // mixed operations are already checked for

      for (int i = 5; i < argc; i += 2) {
        search_strings += kUS;
        search_strings += argv[i];
      }
    }
    msg += op_code;
    msg += search_strings;
    msg += kEoT;



    // write message 4 bytes at a time
    const size_t kWrite_buffer_size = 32;
    char write_buffer[kWrite_buffer_size];
    int bytes_written;


    size_t use_write_buffer_size = kWrite_buffer_size;
    if (msg.size() < kWrite_buffer_size) {
      use_write_buffer_size = msg.size();
    }

    strcpy(write_buffer, msg.c_str());  // NOLINT
    std::string write_buffer_string(write_buffer, use_write_buffer_size);

    bytes_written = write(socket_fd, write_buffer, use_write_buffer_size);

    while (bytes_written > 0) {
      if (write_buffer_string.find(kEoT) != std::string::npos) {  // is found
        bytes_written = 0;
        break;
      }

      strcpy(write_buffer, "");  // NOLINT
      msg = msg.substr(kWrite_buffer_size);
      strcpy(write_buffer, msg.c_str());  // NOLINT

      if (msg.size() < kWrite_buffer_size) {
        use_write_buffer_size = msg.size();
      }

      write_buffer_string.assign(write_buffer, use_write_buffer_size);

      bytes_written = write(socket_fd, write_buffer, use_write_buffer_size);
    }
    if (bytes_written < 0) {
        std::cerr << strerror(errno) << std::endl;

        exit(-1);
    }



    // read all data sent from server
    //  write all lines of text to STDOUT
    int lineCount = 1;
    int totalBytesRead = 0;

    // recieve data from server
    while (true) {
      std::string fullMsg;
      fullMsg = "";
      const size_t kRead_buffer_size = 32;  // read 4 byte increments
      char read_buffer[kRead_buffer_size];
      int bytes_read;

      strcpy(read_buffer, "");  // NOLINT

      bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);
      totalBytesRead += bytes_read;
      fullMsg.append(read_buffer, bytes_read);

      while (bytes_read > 0) {
        // invalid file recieved
        if (strcmp(fullMsg.c_str(), "INVALID FILE") == 0) {
          std::cerr << fullMsg << std::endl;
          exit(-1);
        }
        // end of line
        if (fullMsg.find(kUS) != std::string::npos) {  // is found
          std::cout << lineCount << "\t" << fullMsg << std::endl;
          ++lineCount;
          break;
        }
        // end of file
        if (fullMsg.find(kEoT) != std::string::npos) {  // is found
          bytes_read = 0;
          break;
        }

        bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);
        totalBytesRead += bytes_read;
        fullMsg.append(read_buffer, bytes_read);
      }

      if (bytes_read == 0) {
        std::clog << "BYTES RECIEVED: " << totalBytesRead << std::endl;

        close(socket_fd);
        exit(0);
      } else if (bytes_read < 0) {
        std::cerr << strerror(errno) << std::endl;

        exit(-1);
      }
    }
  }
};


int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout <<
      "USEAGE: ./test-client <csv_server_socket> " <<
      "<path_to_file> <search_expression>" <<
      std::endl;
    return 1;
  }

  // check for mixed operations
  if (argc > 4) {
    std::string op_code;
    if (strcmp(argv[4], "+") == 0) {
      op_code = "OR";
    } else if (strcmp(argv[4], "x") == 0) {
      op_code = "AND";
    }

    for (int i = 6; i < argc; i += 2) {
      std::string cmp_op_code;
      if (strcmp(argv[i], "+") == 0) {
        cmp_op_code = "OR";
      } else if (strcmp(argv[i], "x") == 0) {
        cmp_op_code = "AND";
      }
      if (strcmp(cmp_op_code.c_str(), op_code.c_str()) != 0) {
        std::cerr << "Mixed boolean operations not presently supported"
          << std::endl;
        exit(2);
      }
    }
  }

  const std::string kSocket_path = argv[1];

  const int length = kSocket_path.length();

  char* sock_path = new char[length+1];
  strcpy(sock_path, kSocket_path.c_str());  // NOLINT

  SocketClient client(sock_path);
  client.RunClient(argc, argv);

  return 0;
}
