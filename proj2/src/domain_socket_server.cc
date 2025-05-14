// Copyright Joshua Moorehead
#include "../inc/domain_socket_server.h"




void DomainSocketServer::RunServer() const {
  int sock_fd;  // unnamed socket file descriptor
  int client_req_sock_fd;  // client connect request socket file descriptor

  // (1) create a socket
  //       AF_UNIX -> file system pathnames
  //       SOCK_STREAM -> sequenced bytestream
  //       0 -> default protocol (let OS decide correct protocol)
  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if ( sock_fd < 0 ) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // (2) bind socket to address for the server
  unlink(socket_path_.c_str());  // sys call to delete file if it already
                                  //   exists (using Unix system calls for
                                  //   sockets, no reason to be non-Unix
                                  //   portable now).  :-/
  int success = bind(sock_fd,
                      // sockaddr_un is a Unix sockaddr and so may be cast "up"
                      //   to that pointer type (think of it as C polymorphism)
                      reinterpret_cast<const sockaddr*>(&sock_addr_),
                      // size needs be known due to underlying data layout,
                      //   i.e., there may be a size difference between parent
                      //   and child
                      sizeof(sock_addr_));

  // log errors
  if (success < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // (3) Listen for connections from clients
  size_t kMax_client_conns = get_nprocs_conf() - 1;
  success = listen(sock_fd, kMax_client_conns);
  if (success < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  std::clog << "SERVER STARTED"
  << std::endl << "\tMAX CLIENTS: " << kMax_client_conns << std::endl;

  const char kUS = static_cast<char>(31);
  const char kEoT = static_cast<char>(3);


  const size_t kRead_buffer_size = 32;  // read 4 byte increaments
  char read_buffer[kRead_buffer_size];
  int bytes_read;

  while (true) {  // for each client
    // (4) Accept connection from a client
    client_req_sock_fd = accept(sock_fd, nullptr, nullptr);
    if (client_req_sock_fd  < 0) {
      std::cerr << strerror(errno) << std::endl;
      continue;
    }
    std::string clientMessage = "";
    std::clog << "CLIENT CONNECTED" << std::endl;

    // (5) Receive data from client(s)

    bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
    clientMessage.append(read_buffer, bytes_read);
    while (bytes_read > 0) {
      if (clientMessage.find(kEoT) != std::string::npos) {
        // done reading
        bytes_read = 0;
        break;
      }
      strcpy(read_buffer, ""); //NOLINT
      bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
      clientMessage.append(read_buffer, bytes_read);
    }

    if (bytes_read < 0) {
      std::cerr << strerror(errno) << std::endl;

      exit(-1);
    }

    std::vector v = explode(clientMessage, kUS);
    // puts client message into vector based on delim value

    // now i open csv file, and check if invalid
    std::string fileName = v[0];
    std::string op = v[1];

    std::clog << "PATH: " << fileName << std::endl;
    std::clog << "OPERATION: " << op << std::endl << "SEEKING: ";
    for (int i = 2; i < static_cast<int>(v.size())-2; i++) {
        std::cout << v[i] << ", ";
    }
    std::cout << v[v.size()-2];
    std::cout << std::endl;

    // using path to open file
    std::ifstream csv_file;
    int totalBytes(0);
    csv_file.open(fileName.c_str());
    if (!csv_file) {
      std::string invalid = "INVALID FILE";
      write(client_req_sock_fd, invalid.c_str(), invalid.size());
      close(client_req_sock_fd);
    }
    // checked if invalid, need to get each line of file
    if (csv_file.is_open()) {
      std::string line;
      while (std::getline(csv_file, line)) {
        bool matchingLine = false;
        if (op.compare("n/a") == 0) {
          if (line.find(v[2]) != std::string::npos) {
            matchingLine = true;
          }
        } else if (op.compare("OR") == 0) {
          for (int i = 2; i < static_cast<int>(v.size())-1; i++) {
            if (line.find(v[i]) != std::string::npos) {
              matchingLine = true;
              break;
            }
          }
        } else if (op.compare("AND") == 0) {
          for (int i = 2; i < static_cast<int>(v.size())-1; i++) {
            if (line.find(v[i]) != std::string::npos) {
              matchingLine = true;
            } else {
              matchingLine = false;
              break;
            }
          }
        }
        // at this point, we know if line is returnable
        if (matchingLine) {
          line.push_back(kUS);
          const ssize_t kWrite_buffer_size = 32;
          char write_buffer[kWrite_buffer_size];
          int bytes_written;
          std::string buffer_string_sent = "";
          size_t new_buffer_size = kWrite_buffer_size;
          strcpy(write_buffer, ""); // NOLINT

          // checking if message size is less than buffer
          if (line.size() < kWrite_buffer_size) {
            new_buffer_size = line.size();
          }
          strncpy(write_buffer, line.c_str(), new_buffer_size);
          bytes_written = write(client_req_sock_fd, write_buffer, new_buffer_size); // NOLINT
          usleep(20000);
          totalBytes += bytes_written;
          buffer_string_sent.append(write_buffer, new_buffer_size);
          // std::cout << buffer_string_sent << std::endl;
          // std::cout << write_buffer << std::endl;
          while (bytes_written > 0) {
            if (buffer_string_sent.find(kUS) != std::string::npos) {
              bytes_written = 0;
              break;
            }
            strcpy(write_buffer, ""); // NOLINT
            // erasing buffer size off of string
            line.erase(0, new_buffer_size);
            // checking if message size is less than buffer
            if (line.size() < kWrite_buffer_size) {
              new_buffer_size = line.size();
            }
            strncpy(write_buffer, line.c_str(), new_buffer_size);
            // write() is equivalent to send() with no flags in send's 3rd param
            bytes_written = write(client_req_sock_fd, write_buffer, new_buffer_size); // NOLINT
            usleep(20000);
            totalBytes += bytes_written;
            buffer_string_sent.append(write_buffer, new_buffer_size);
          }

          if (bytes_written < 0) {
            std::cerr << strerror(errno) << std::endl;

            exit(-1);
          }
        }
      }
      // now that matching line has been sent buffer by buffer,
      // we can close the file and send a character to done
      csv_file.close();
      std::string endChar{kEoT};
      totalBytes += write(client_req_sock_fd, endChar.c_str(), endChar.size());
      std::cout << "BYTES SENT: " << totalBytes << std::endl;
    }
  }
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




int main(int argc, char** argv) {  //  takes in command line arguments
  DomainSocketServer server(argv[1]);
  server.RunServer();
  return 0;
}
