// Copyright Joshua Moorehead 2023
//
//
#ifndef MMAP_FSTREAM_H_
#define MMAP_FSTREAM_H_

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ios>
#include <string>


namespace mem_map {

class fstream {
 public:
  // Creates Memory-mapped file stream obj without file name; file name must be
  //   specified in call to Open.
  //
  //   You may use default parameter values to allow following constructors to
  //   absorb this one
  //
  fstream();


  // Creates Memory-mapped file stream obj with file name
  //
  //   Open mode is std::ios_base::in | std::ios_base:: out by default.
  //
  //   Result of constructor can be determined by is_open
  //
  //   You may use default parameter values to allow following constructor to
  //   absorb this one
  //
  explicit fstream(const std::string& fname);


  // Creates Memory-mapped file stream obj with file name and open mode
  //
  //   Result of constructor can be checked with is_open
  //
  //   Must handle any combination of modes
  //     - std::ios_base::ate  (open with cursor at the end of the file)
  //     - std::ios_base::in  (open with read privileges)
  //     - std::ios_base::out  (open with write privileges)
  //
  fstream(const std::string& fname, std::ios_base::openmode mode);

  // Attempts to open file given by file_name
  //
  //   Open mode is std::ios_base::in | std::ios_base::out by default
  //
  //   Result can be determined by is_open
  //
  //   Does nothing if file is already open
  //
  //   You may use default parameter values to overload following method to
  //   absorb this one
  //
  //   Simplifying assumption: no file will ever be larger than 2^12 bytes
  //
  void open(const std::string& fname);


  // Attempts to open file given by name with open mode specified by mode
  //
  //   Result can be determined by is_open
  //
  //   Does nothing if file is already open
  //
  //   Must handle any combination of modes
  //     - std::ios_base::ate  (open with cursor at the end of the file)
  //     - std::ios_base::in  (open with read privileges)
  //     - std::ios_base::out  (open with write privileges)
  //
  //   Simplifying assumption: no file will ever be larger than 2^12 bytes
  //
  void open(const std::string& fname, std::ios_base::openmode mode);


  // Attempts to close an open file
  //
  //   Does nothing if file is already open
  //
  //   Simplifying assumption: no file will ever be larger than 2^12 bytes
  //
  void close();


  // Returns file's open state
  //
  bool is_open() const;


  // Returns file's current size; may change dynmically due to fstream::put
  //
  std::size_t size() const;


  // Retrieves "next" character from file and updates cursor
  //
  //  Returns '\0' when no further characters exist
  //
  //  This method must not modify a file; only updates cursor position if not
  //  at end of file
  //
  char get();


  // Writes character at "next" space in the file and updates cursor
  //
  //  This method may increase the size of a file
  //
  fstream& put(char c);

 private:
    std::size_t size_;
    std::size_t index;
    const int bitPageSize = 4096;
    int fd;
    const char* filename;
    char* address;
};

}  // namespace mem_map

#endif  // MMAP_FSTREAM_H_
