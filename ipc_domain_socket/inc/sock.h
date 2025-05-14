#ifndef INC_SOCK_H_
#define INC_SOCK_H_

#include <sys/socket.h>  // Unix header for sockets, using socket
#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>  // Unix standard header, using close

#include <cassert>  // using assert
#include <cerrno>  // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror

#include <string>
#include <iostream>


class UnixDomainSocket {
 public:
  ///
  /// Initializes the socket address struct (::sockaddr_un)
  ///
  /// @param socket_path A null-terminated C-string containing the shared path
  ///                    name for socket.
  /// @param abstract Defaults to abstract socket path names, pass false for
  ///                 non-abstract naming.
  ///
  explicit UnixDomainSocket(const char *socket_path, bool abstract = true) {
    socket_path_ = std::string(socket_path);  // std::string manages char *

    sock_addr_ = {};  // init struct (replaces memset from C)
    sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                      //   of internet domain socket)
    if (abstract)
      // leaving leading null char sets abstract socket
      strncpy(sock_addr_.sun_path + 1,           // strncpy to limit copy for
              socket_path,                       //   portability
              sizeof(sock_addr_.sun_path) - 1);  // -2 for leading/trailing \0s
    else
      // copy string from socket path without leading \0
      strncpy(sock_addr_.sun_path,               // strncpy to limit copy for
              socket_path,                       //   portability
              sizeof(sock_addr_.sun_path));  //
  }

 protected:
  ::sockaddr_un sock_addr_;  // socket address from sys/un.h

  std::string socket_path_;  // std::string stores socket_path (no raw pointers)
};
#endif  // INC_SOCK_H_
