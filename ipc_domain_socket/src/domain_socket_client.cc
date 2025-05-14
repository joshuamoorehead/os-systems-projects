// Written by Joshua Moorehead 3/23 Copyright
#include "../inc/domain_socket_client.h"




void DomainSocketClient::RunClient(std::string msg) const {
  // (1) open nameless Unix socket
  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // (2) connect to an existing socket
  int success = connect(socket_fd,
                        // sockaddr_un is a Unix sockaddr
                        reinterpret_cast<const sockaddr*>(&sock_addr_),
                        sizeof(sock_addr_));
  if (success < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  std::clog << "SERVER CONNECTION ACCEPTED" << std::endl;

  const char kUS = static_cast<char>(31);
  const char kEoT = static_cast<char>(3);

  // (3) write to socket
  const ssize_t kWrite_buffer_size = 32;
  char write_buffer[kWrite_buffer_size];
  int bytes_written = 0;
  size_t new_buffer_size = kWrite_buffer_size;
  // checking if message size is less than buffer
  if (msg.size() < kWrite_buffer_size) {
    new_buffer_size = msg.size();
  }
  strcpy(write_buffer, msg.c_str());  // NOLINT
    std::string buffer_string_sent(write_buffer, new_buffer_size);

    bytes_written = write(socket_fd, write_buffer, new_buffer_size);

    while (bytes_written > 0) {
      if (buffer_string_sent.find(kEoT) != std::string::npos) {
        bytes_written = 0;
        break;
      }

      strcpy(write_buffer, "");  // NOLINT
      msg = msg.substr(kWrite_buffer_size);
      strcpy(write_buffer, msg.c_str());  // NOLINT

      if (msg.size() < kWrite_buffer_size) {
        new_buffer_size = msg.size();
      }

      buffer_string_sent.assign(write_buffer, new_buffer_size);

      bytes_written = write(socket_fd, write_buffer, new_buffer_size);
    }
  if (bytes_written < 0) {
    std::cerr << strerror(errno) << std::endl;

    exit(-1);
  }

  std::cout << "DONE WRITING" << std::endl;

  // time to read back from the server
  int lineNumber(1);
  int totalBytes(0);


  while (true) {
    const size_t kRead_buffer_size = 32;  // read 4 byte increaments
    char read_buffer[kRead_buffer_size];
    int bytes_read(0);
    std::string serverMessage = "";
    // need to reset the buffer
    strcpy(read_buffer, ""); //NOLINT

    bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);
    totalBytes += bytes_read;
    serverMessage.append(read_buffer, bytes_read);

    while (bytes_read > 0) {
      if (serverMessage.find(kEoT) != std::string::npos) {
        std::cout << "Done reading..." << std::endl;
        bytes_read = 0;
        break;
      }
      if (serverMessage.compare("INVALID FILE") == 0) {
        std::cerr << "INVALID FILE" << std::endl;
        exit(-1);
      }
      if (serverMessage.find(kUS) != std::string::npos) {
        std::cout << lineNumber << "\t" << serverMessage << std::endl;
        lineNumber++;
        break;
      }
      bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);
      totalBytes += bytes_read;
      serverMessage.append(read_buffer, bytes_read);
    }

    if (bytes_read < 0) {
      std::cerr << strerror(errno) << std::endl;

      exit(-1);
    }
    if (bytes_read == 0) {
      std::clog << "BYTES RECEIVED: " << totalBytes << std::endl;
      close(socket_fd);
      exit(0);
    }
  }
}


std::string parseCommandLine(int argc, char** argv) {
  const char kUS = static_cast<char>(31);
  const char kEoT = static_cast<char>(3);
  int checkerOr(0);
  int checkerAnd(0);
  int counter(0);
  std::string message = "";

  // client message order -> file name, op code, search terms
  message.append(argv[2]);
  message += (kUS);
  for (int i = 3; i < argc; i++) {  // checking for mixed op code
    if (strcmp(argv[i], "+") == 0)
      checkerOr++;
    if (strcmp(argv[i], "x") == 0)
      checkerAnd++;
    if (checkerOr > 0 && checkerAnd > 0) {
      std::cerr <<
      "Mixed boolean operations not presently supported" << std::endl;
      exit(-1);
    }
  }
  // adding all the opcode now to the message
  for (int i = 3; i < argc; i++) {
    if (((strcmp(argv[i], "+") == 0 )||
    strcmp(argv[i], "x") == 0) && counter == 0) {
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
  for (int i= 3; i < argc; i+=2) {
    if (strcmp(argv[i], "+") != 0 ||
    strcmp(argv[i], "x") != 0) {
      message.append(argv[i]);
      message += (kUS);
    }
  }
  message += (kEoT);
  return message;
}






int main(int argc, char** argv) {
    // parse command line args and store in a string

    DomainSocketClient client(argv[1]);

    std::string message = parseCommandLine(argc, argv);

    client.RunClient(message);

    return 0;
}

