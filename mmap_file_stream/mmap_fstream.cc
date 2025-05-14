// Copyright Joshua Moorehead 2023

#include "./mmap_fstream.h"



mem_map::fstream::fstream() {
    size_ = 0;
    index = 0;
    fd = -1;
    filename = nullptr;
    address = nullptr;
}

mem_map::fstream::fstream(const std::string& fname) : fstream() {
    open(fname);
}

mem_map::fstream::fstream(const std::string& fname,
                            std::ios_base::openmode mode) : fstream() {
    open(fname, mode);
}

void mem_map::fstream::open(const std::string& fname) {
    if (!is_open()) {  // check if file is open, if it is do nothing
        filename = fname.c_str();
        int fd_flags = O_RDWR;
        int mmap_flags = PROT_READ | PROT_WRITE;
        // opens file by given fname
        fd = ::open(fname.c_str(), fd_flags | O_CREAT, 0664);
        if (!is_open()) {
            printf("open err: %s", strerror(errno));
            exit(1);
        }
        struct ::stat file_status;
        ::fstat(fd, &file_status);  // struct ::stat is an output parameter
        size_ = file_status.st_size;
        // MEMORY MAP OPEN FILE
        address = static_cast<char *>(mmap(NULL,
                                bitPageSize,
                                mmap_flags,
                                MAP_SHARED,
                                fd,
                                0));
        if (address == MAP_FAILED) {
            printf("mmap err: %s", strerror(errno));
            exit(1);
        }
        if (::access(filename, W_OK) == 0) {
            int trun = truncate(filename, bitPageSize);
            if (trun < 0) {
                printf("trunc err: %s", strerror(errno));
            }
        }
    }
}

void mem_map::fstream::open(const std::string& fname,
                             std::ios_base::openmode mode) {
    if (!is_open()) {  // check if file is open, if it is do nothing
        int fd_flags;
        int mmap_flags;
        // decipher the correct mode
        if ((mode & (std::ios_base::in | std::ios_base::out))
             == (std::ios_base::in | std::ios_base::out)) {
            mmap_flags = PROT_READ | PROT_WRITE;
            fd_flags = O_RDWR;
        } else if ((mode & std::ios_base::in) == std::ios_base::in) {
            mmap_flags = PROT_READ;
            fd_flags = O_RDONLY;
        } else if ((mode & std::ios_base::out) == std::ios_base::out) {
            mmap_flags = PROT_WRITE;
            fd_flags = O_WRONLY;
        }
        // opens file by given fname
        fd = ::open(fname.c_str(), fd_flags | O_CREAT, 0664);
        if (!is_open()) {
            printf("open err: %s", strerror(errno));
            exit(1);
        }
        struct ::stat file_status;
        ::fstat(fd, &file_status);  // struct ::stat is an output parameter
        size_ = file_status.st_size;

        filename = fname.c_str();

        if ((mode & std::ios_base::ate) == std::ios_base::ate)
            index = size_;

        // MEMORY MAP OPEN FILE
        address = static_cast<char *>(mmap(NULL,
                                bitPageSize,
                                mmap_flags,
                                MAP_SHARED,
                                fd,
                                0));
        if (address == MAP_FAILED) {
            printf("mmap err: %s", strerror(errno));
            exit(1);
        }
        if (::access(filename, W_OK) == 0) {
            int trun = truncate(filename, bitPageSize);
            if (trun < 0) {
                printf("trunc err: %s", strerror(errno));
            }
        }
    }
}

void mem_map::fstream::close() {
    if (is_open()) {
        if (::close(fd) < 0) {
            printf("close err: %s", strerror(errno));
            exit(1);
        }
        // truncate down to size
        if (::access(filename, W_OK) == 0) {
            if (truncate(filename, size_) < 0) {
                printf("trunc err: %s", strerror(errno));
            }
        }
        // SAVE TO DISK
        int msync_sig = msync(address, bitPageSize, MS_SYNC);
        if (msync_sig < 0) {
            printf("msync err: %s", strerror(errno));
        }
        // munmap it
        munmap(address, bitPageSize);
        fd = -1;
    }
}

bool mem_map::fstream::is_open() const {
    return (fd > 0);
}

std::size_t mem_map::fstream::size() const {
    return size_;
}

char mem_map::fstream::get() {
    // check if read access before reading
    if (::access(filename, R_OK) == 0) {
        if (index == size_) {
            return '\0';
        } else if (index > size_) {
            printf("index greater than size: %s", strerror(errno));
        }
        // READ FROM FILE
        char i = address[index];
        index++;
        return i;
    }
    return '\0';
}

mem_map::fstream &mem_map::fstream::put(char c) {
    // check if write access before putting
    if (::access(filename, W_OK) == 0) {
            size_++;
            address[index] = c;
            index++;
    }
    return *this;
}
