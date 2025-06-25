# -O2 - for optimization
# -Wall - for better warnings
# -std - for version specification
# 	c2x - version 23 for c
flags=-O2 -Wall -std=c2x

all: clean server

# server executable w/ server.o as a dependency
# cc - compiler
# ${flags} - allow flags
# $^ - source file
# -o - the binary flag(?)
# $@ - the target
# ${ldflags} - helps with linkage
server: server.o
	cc ${flags} $^ -o $@ ${ldflags}

# Create the server.o binary w/ the dependency server.c
# cc - compiler
# ${flags} - allow flags
# -c - compile to a binary file
# $^ - specify the sourcefile  
server.0: server.c
	cc: ${flags} -c $^

# Remove any binary files and executables (server.o and server)
clean:
	rm -f *.o server