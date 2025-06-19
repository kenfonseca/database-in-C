# -O2 - for optimization
# -Wall - for better warnings
# -std - for version specification
# 	c2x - version 23 for c
flags=-O2 -Wall -std=c2x

all: clean IMDB

# IMDB executable w/ IMDB.o as a dependency
# cc - compiler
# ${flags} - allow flags
# $^ - source file
# -o - the binary flag(?)
# $@ - the target
# ${ldflags} - helps with linkage
IMDB: IMDB.o
	cc ${flags} $^ -o $@ ${ldflags}

# Create the IMDB.o binary w/ the dependency IMDB.c
# cc - compiler
# ${flags} - allow flags
# -c - compile to a binary file
# $^ - specify the sourcefile  
IMDB.0: IMDB.c
	cc: ${flags} -c $^

# Remove any binary files and executables (IMDB.o and IMDB)
clean:
	rm -f *.o IMDB