TARGET = bin/dbview
SRC = $(wildcard src/*.c)  # wild card expands the files in src dir, so SRC will be "src/main.c src/test.c"
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC)) # subsstitubes .c with .o here,

run: clean default
		./$(TARGET) -f ./mynewdb.db -n
		./$(TARGET) -f ./mynewdb.db -a "nishan,jadibuti,10"

default: $(TARGET)



clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

# $@ is the name of the target, in this case $(TARGET) = bin/dbview\
	# $^: Includes all prerequisites (all object files: obj/main.o, etc.).

build: $(OBJ)
	gcc -o test $?

$(TARGET): $(OBJ)
	gcc -o $@ $?

# -c: Compiles the source file to an object file without linking.
# $<: The first prerequisite (i.e., the current .c file being compiled).
# -o $@: Outputs the result to the corresponding .o file.
# -Iinclude: Includes the include/ directory for header files
obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude

# In this make file
# Since we use -Iinclude, compiler automatically changes
# #include "myheader.h" to #include "../include/myheader.h"
# there is separeion here in OBJ step, object files are created
# which means each C code is compiled into machine code, so file.c functions
# have definitons, which main does not know yet
# during the file compilation step gcc -o output file.o parse.o main.o -I$(pwd)/include
# all of the resolution is done
