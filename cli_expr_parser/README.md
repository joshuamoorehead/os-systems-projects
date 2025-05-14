# Joshua Moorehead Calculator

This project's use is to calculate some math after it receives input from the command line such as 5 + 3 = 8 using a stack

## main.cc

- Contains main() - includes calculate.cc, then uses its functions to calculate given input from argc and argv

## calculate.cc

- includes calculate.h 
- Contains doMath() - returns the result of two numbers and a single operator
- Contains populateStacks() - reads all input values and prepares the number stack and operator stack for final calculations
- Contains calculateAll() - finishes whatevers left on stacks after populateStacks()

## calculate.h

- Contains #include <iostream>, as is needed for printing to the output stream
- Contains #include <stack>, as is needed for the use of stack
- Contains std namespace for stack functions

## Makefile

- Use the command 'make' to compile and link the project
- Produces 'calculate' as an executable 
- Use the command 'make clean' to remove all .o files
