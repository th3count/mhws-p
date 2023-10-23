# Compiler and linker options
CC = gcc
CFLAGS = -Wall -std=c99 -g
LIBS = -lwiringPi -lncurses

# Source file and object file directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Executable name
EXECUTABLE = $(BIN_DIR)/irrigation_control

# Compilation rules
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Phony targets
.PHONY: clean

clean:
	rm -f $(OBJS) $(EXECUTABLE)

# Default target
all: $(EXECUTABLE)

# Dependencies
$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# Remove built-in rules
.SUFFIXES: