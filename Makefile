CC = clang
CFLAGS = -Wall -Wextra
OPT = -O0

BIN_DIR = bin
SRC_DIR = src
BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

MAIN = main.c

# Output library
LIB = libOskServer.a

# Default target
all: lib

lib: $(LIB)

debug: OPT = -O0
debug: CFLAGS += -g
debug: clean main

main: lib
	$(CC) $(CFLAGS) $(OPT) main.c $(LIB) -o main.exe


# Create static library
$(LIB): $(OBJ)
	ar rcs $@ $^

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

# Clean build artifacts
clean:
	rmdir /s /q $(BUILD_DIR) 
	del $(LIB)

# Rebuild everything
re: clean all



