// Copyright 2023 Clay Crews

#ifndef _PROJ2_INC_SERVER_H_
#define _PROJ2_INC_SERVER_H_

  #include <unistd.h>
  #include <sys/sysinfo.h>

  #include <iostream>
  #include <fstream>
  #include <sstream>
  #include <vector>
  #include <cassert>
  #include <cerrno>
  #include <cstddef>
  #include <cstdlib>
  #include <cstring>
  #include <string>

  #include "./UnixDomainSocket.h"

  void split_str(std::string const &str,
                const char delim,
                std::vector <std::string> &out);
  
  ssize_t writeCheck(ssize_t written);


#endif  // _PROJ2_INC_SERVER_H_

