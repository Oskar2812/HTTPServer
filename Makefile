CC = clang
CFLAGS = -Wall -Wextra
LINK_FLAGS = -lws2_32
OPT = -O0

BIN_DIR = bin
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

MAIN = main.c
TEST_BIN = tests.exe

# Output library
LIB = libOskServer.a

.PHONY: tests

# Default target
all: lib

lib: $(LIB)

debug: OPT = -O0
debug: CFLAGS += -g
debug: clean main

main: lib
	$(CC) $(CFLAGS) $(OPT) main.c $(LIB) -o app.exe $(LINK_FLAGS)

run_main: main
	./app


tests:
	$(CC) $(CFLAGS) $(OPT) $(TEST_DIR)/TestRunner.c -o $(TEST_BIN)

run_tests: tests
	./$(TEST_BIN)


# Create static library
$(LIB): $(OBJ)
	ar rcs $@ $^

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@


$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.c
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@


# Clean build artifacts
clean:
	-rmdir /s /q $(BUILD_DIR) 
	del $(LIB)
	del app.exe

# Rebuild everything
re: clean all



