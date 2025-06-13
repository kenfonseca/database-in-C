tree.h - Used to keep definitions
* "Header" file

tree.c - The programming implementation

Makefile - Used to compile/recompile easily
* Error 1 means that there is no code for the makefile to compile
```
# tree - the executable 
# tree.o - binary used to create the executable
# cc - the compiler
# $^ - the source file
# -o - flag for output files
# $@ - target output file
# ${ldflags} - used for the linking process
tree: tree.o
	cc ${flags} $^ -o $@ ${ldflags}

# tree.o - the binary to be compiled
# tree.c - our program used to compile the binary
# cc - the compiler to be used
# ${flags} - any flags to be used on use of command
# -c $^ - The name of the input file
tree.o: tree.c
	cc ${flags} -c $^

# Remove old files and the tree in binary
clean:
	rm -f *.o tree
```