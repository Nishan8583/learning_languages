CC = gcc

# These are flags passed to the compiler:
#-Wall: Enable most common warnings.
#-Wextra: Enable even more warnings.
#-Iinclude: Tells the compiler to look in the include/ directory for header files.
CFLAGS = -Wall -Wextra -Iinclude

# our source files
SRC = ./main.c src/copy_file.c


#This is a make substitution pattern.
#It replaces every .c in SRC with .o — so:
#	main.c → main.o
# src/copy_file.c → src/copy_file.o
# we are just renaming here, turns out we do not need to compile to object file. There is an implicit rule
# That means:

#If main.o is a dependency

#And main.c exists

#And you didn’t define how to build .o files
#“To get main.o, compile main.c.”
# Implicit rule:
#%.o : %.c
#$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

OBJ = $(SRC:.c=.o)

# Final executable name
TARGET = sync_tool


all: $(TARGET)

# gcc main.o src/copy_file.o -o sync_tool
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@


clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all cle
	return 0;
