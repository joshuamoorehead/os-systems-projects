#Written by Joshua Moorehead Feb 5 2023

cc = g++ 
path = ..
standard = c++17
TARGET = calculate

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation

link = $(cc) $(flags) -o $(TARGET) main.o  # final linked build to binary executable

compile =  $(cc) $(flags) -c main.cc # compilation to intermediary .o files
#executables

$(TARGET) : main.o 
	$(cc) $(flags) -o $(TARGET) main.o

main.o : main.cc calculator.cc calculator.h
	$(cc) $(flags) -c main.cc

#$(TARGET) : main.o 
#	$(link)

#main.o : main.cc calculator.cc calculate.h
#	$(compile) 

clean:
	$(RM) *.o