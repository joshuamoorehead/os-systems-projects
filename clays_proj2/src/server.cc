// Copyright 2023 Clay Crews

#include "../inc/server.h"


void split_str(std::string const &str,
                const char delim,
                std::vector <std::string> &out) {  // NOLINT
  std::stringstream s(str);

  std::string s2;
  while (std::getline(s, s2, delim)) {
      out.push_back(s2);
  }
}

ssize_t writeCheck(ssize_t written) {
  if (written < 0) {
    std::cerr << strerror(errno) << std::endl;

    exit(-1);
  }

  if (written == 0) {
    std::clog << "Client has disconnected!" << std::endl;
    exit(-2);
  }

  return written;
}

class SocketServer : public UnixDomainSocket {
 public:
  using ::UnixDomainSocket::UnixDomainSocket;

  void RunServer() const {
    int sock_fd;  // unnamed socket file descriptor
    int client_req_sock_fd;  // client connect request socket file descriptor

    // create a socket
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // bind socket to address for the server
    unlink(socket_path_.c_str());

    int success = bind(sock_fd,
                        reinterpret_cast<const sockaddr*>(&sock_addr_),
                        sizeof(sock_addr_));

    // log errors
    if (success < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // listen for client connections
    size_t kMax_client_conns = get_nprocs_conf() - 1;

    success = listen(sock_fd, kMax_client_conns);
    if (success < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    std::clog << "SERVER STARTED" << std::endl <<
        "\tMAX CLIENTS: " << kMax_client_conns << std::endl;

    const char kUS = static_cast<char>(31);
    const char kEoT = static_cast<char>(3);

    while (true) {  // for individual client
      const size_t kRead_buffer_size = 32;
      char read_buffer[kRead_buffer_size];
      int bytes_read;

      // accept client connection
      client_req_sock_fd = accept(sock_fd, nullptr, nullptr);
      if (client_req_sock_fd < 0) {
        std::cerr << strerror(errno) << std::endl;
        continue;
      }

      std::clog << "CLIENT CONNECTED" << std::endl;

      // recieve data from clients

      // read data message from client
      std::string fullMsg = "";

      bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
      fullMsg.append(read_buffer, bytes_read);

      while (bytes_read > 0) {
        if (fullMsg.find(kEoT) != std::string::npos) {  // EoT is found
          bytes_read = 0;
          break;
        }

        strcpy(read_buffer, "");  // NOLINT
        bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
        fullMsg.append(read_buffer, bytes_read);
      }
      if (bytes_read < 0) {
        std::cerr << strerror(errno) << std::endl;

        exit(-1);
      }

      // pop off EoT char
      fullMsg.pop_back();

      // parse data from client
      const char delim = kUS;
      std::vector <std::string> out;
      split_str(fullMsg, delim, out);

      std::string filename = out.at(0);
      std::string op_code = out.at(1);

      std::vector <std::string> tokens;

      tokens.push_back(out.at(2));
      std::string tokens_string = out.at(2);

      for (size_t i = 3; i < out.size(); ++i) {
        tokens.push_back(out.at(i));
        tokens_string += ", " + out.at(i);
      }

      std::clog << "PATH: " << filename << std::endl;

      std::clog << "OPERATION: " << op_code << std::endl <<
        "SEEKING: " << tokens_string << std::endl;


      // use path to open file
      std::ifstream file;

      int totalBytesWritten = 0;

      // send invalid file if cannot be opened
        // close client connection
        // wait for next connection
      file.open(filename.c_str());
      if (!file) {
        std::string invalid_file_msg = "INVALID FILE";

        writeCheck(write(client_req_sock_fd,
                          invalid_file_msg.c_str(),
                          invalid_file_msg.size()));

        close(client_req_sock_fd);

        // now will move to waiting for next connection
      }



      if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
          bool return_line = false;

          if (strcmp(op_code.c_str(), "n/a") == 0) {
            if (line.find(tokens[0]) != std::string::npos) {
              return_line = true;
            }
          } else if (strcmp(op_code.c_str(), "OR") == 0) {
            for (size_t i = 0; i < tokens.size(); ++i) {
              if (line.find(tokens[i]) != std::string::npos) {
                return_line = true;
                break;
              }
            }
          } else if (strcmp(op_code.c_str(), "AND") == 0) {
            for (size_t i = 0; i < tokens.size(); ++i) {
              if (line.find(tokens[i]) != std::string::npos) {
                return_line = true;
              } else {
                return_line = false;
                break;
              }
            }
          }

          if (return_line) {
            line.push_back(kUS);

            // write message 4 bytes at a time
            const size_t kWrite_buffer_size = 32;
            char write_buffer[kWrite_buffer_size];
            int bytes_written;
            size_t use_write_buffer_size;
            std::string write_buffer_string;


            use_write_buffer_size = kWrite_buffer_size;
            if (line.size() < kWrite_buffer_size) {
              use_write_buffer_size = line.size();
            }

            strcpy(write_buffer, line.c_str());  // NOLINT
            write_buffer_string.append(write_buffer, use_write_buffer_size);

            bytes_written = write(client_req_sock_fd,
                                  write_buffer,
                                  use_write_buffer_size);
            usleep(500);
            totalBytesWritten += bytes_written;

            while (bytes_written > 0) {
              // Unit Seperator char found in string
              if (write_buffer_string.find(kUS) != std::string::npos) {
                bytes_written = 0;
                break;
              }

              strcpy(write_buffer, "");  // NOLINT
              line = line.substr(kWrite_buffer_size);
              strcpy(write_buffer, line.c_str());  // NOLINT

              if (line.size() < kWrite_buffer_size) {
                use_write_buffer_size = line.size();
              }

              write_buffer_string.assign(write_buffer, use_write_buffer_size);

              bytes_written = write(client_req_sock_fd,
                                    write_buffer,
                                    use_write_buffer_size);
              usleep(500);
              totalBytesWritten += bytes_written;
            }
            if (bytes_written < 0) {
              std::cerr << strerror(errno) << std::endl;

              exit(-1);
            }
          }
        }

        std::string end = "";
        end.push_back(kEoT);
        totalBytesWritten += writeCheck(write(client_req_sock_fd,
                                              end.c_str(),
                                              end.size()));

        std::cout << "BYTES SENT: " << totalBytesWritten << std::endl;

        file.close();
      }
    }
  }
};


int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "USEAGE: ./text-server <csv_server_socket>" << std::endl;
    return 1;
  }
  const std::string kSocket_path = argv[1];

  const int length = kSocket_path.length();

  char* sock_path = new char[length+1];
  strcpy(sock_path, kSocket_path.c_str());  // NOLINT

  SocketServer server(sock_path );
  server.RunServer();

  return 0;
}
