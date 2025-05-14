# OS Systems Projects

This repository contains a series of C++ systems programming projects developed by Joshua Moorehead. Each project explores foundational operating system concepts such as interprocess communication (IPC), memory-mapped file I/O, and expression parsing from the command line.

---

## 🔧 Projects

### `cli_expr_parser/`
A command-line expression parser that evaluates arithmetic input using custom operand and operator stacks. Demonstrates manual parsing, stack-based evaluation, and expression tree reduction logic.

### `ipc_domain_socket/`
Client-server IPC system using Unix domain sockets to query a CSV file. The server handles multiple clients concurrently and performs line-by-line parsing and filtering of data.

### `ipc_shared_memory/`
Interprocess communication using POSIX shared memory and named semaphores. Supports synchronized memory access across multiple clients querying shared bank data from a centralized server.

### `mmap_file_stream/`
A custom fstream-like implementation using the `mmap()` system call. Enables reading/writing of files in memory as byte streams with random access, including support for seek, put, and get operations.

---

## 🛠 Build & Run

Each folder contains its own `Makefile` and README instructions. To build a project:

```bash
cd project_folder/
make
./<executable>
```

Clean up with:

```bash
make clean
```

---

## 🧠 Topics Covered

- Unix domain sockets
- POSIX shared memory and semaphores
- `mmap()` file handling
- Stack-based arithmetic parsing
- Command-line parsing and string tokenization
- Multi-file C++ compilation and linking

---

## License

All code in this repository is original and written by Joshua Moorehead. These projects are provided for educational and portfolio purposes only. No university-distributed boilerplate or instructional content is included.
