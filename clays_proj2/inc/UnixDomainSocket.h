// Copyright 2023 Clay Crews

#ifndef _PROJ2_INC_UNIXDOMAINSOCKET_H_
#define _PROJ2_INC_UNIXDOMAINSOCKET_H_

  #include <sys/socket.h>
  #include <sys/un.h>
  #include <string>

  class UnixDomainSocket {
   public:
    explicit UnixDomainSocket(const char *socket_path, bool abstract = true) {
      socket_path_ = std::string(socket_path);

      sock_addr_ = {};
      sock_addr_.sun_family = AF_UNIX;

      if (abstract) {
        strncpy(sock_addr_.sun_path + 1,
                socket_path,
                sizeof(sock_addr_.sun_path) - 1);
      } else {
        strncpy(sock_addr_.sun_path,
                socket_path,
                sizeof(sock_addr_.sun_path));
      }
    }

   protected:
    ::sockaddr_un sock_addr_;   // socket address from sys/un.h

    std::string socket_path_;   // std::string stores socket_path
                               // (no raw pointers)
  };

#endif  // _PROJ2_INC_UNIXDOMAINSOCKET_H_
