// Written by Joshua Moorehead 3/23 Copyright
#ifndef INC_DOMAIN_SOCKET_CLIENT_H_
#define INC_DOMAIN_SOCKET_CLIENT_H_
#include <sys/sysinfo.h>
#include <string>
#include "../inc/sock.h"


class DomainSocketClient : public UnixDomainSocket {
 public:
  using ::UnixDomainSocket::UnixDomainSocket;

  void RunClient(std::string msg) const;
};
#endif  // INC_DOMAIN_SOCKET_CLIENT_H_
