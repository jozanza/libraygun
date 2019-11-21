# Silence command output by default
.SILENT:

# We're just using make as a task runner
.PHONY: all build run test

# Library name
BIN = raygun

# C compiler
CC := gcc

# Debugger
DB := lldb

# Source files
SRC := $(BIN).c scene.c timer.c

# Source file & test files
TST := $(SRC) $(BIN)_test.c

# System libraries (be sure to have these installed)
LIB := \
	-lraylib

# Compiler flags
CFLAGS := \
	-std=c11

# Linker flags
LDFLAGS :=

# Builds the archive
build:
	clear
	rm -rf *.o *.a
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -c
	ar rcs lib$(BIN).a *.o
	echo "Built lib$(BIN).a"

# Copies the library and header to the standard locations
install: build
	cp $(BIN).h /usr/local/include
	cp lib$(BIN).a /usr/local/lib
	echo "Installed lib$(BIN).a"

# Runs test suite
test:
	clear
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIB) $(TST) -o $(BIN)_test
	./$(BIN)_test
	rm -rf ./$(BIN)_test
