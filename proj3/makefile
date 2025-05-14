#Copyright Joshua Moorehead 4/4

cc = g++ 
path = ..
standard = c++17
#TARGET = calculate

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation
flage += -pthread

link = $(cc) $(flags) -o  # final linked build to binary executable
compile =  $(cc) $(flags) -c # compilation to intermediary .o files

all: csv-client csv-server

csv-client: client.o named_semaphore.o
	$(link) $@ $^

csv-server: server.o named_semaphore.o
	$(link) $@ $^

#OBJECTS	

client.o: src/client.cc 
	$(compile) $@ $<

server.o: src/server.cc 
	$(compile) $@ $<

named_semaphore.o: src/named_semaphore.cc
	$(compile) $@ $<


clean:
	$(RM) *.o csv-client csv-server
