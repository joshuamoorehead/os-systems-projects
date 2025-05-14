// Copyright Joshua Moorehead CSCE311SP23
#ifndef INC_DOMAIN_SOCKET_SERVER_H_
#define INC_DOMAIN_SOCKET_SERVER_H_
#include <sys/sysinfo.h>
#include <vector>
#include <fstream>
#include <string>
#include "../inc/sock.h"


class DomainSocketServer : public UnixDomainSocket {
 public:
  using ::UnixDomainSocket::UnixDomainSocket;

  void RunServer() const;
};

const std::vector<std::string> explode(const std::string& s, const char& c);

#endif  // INC_DOMAIN_SOCKET_SERVER_H_



