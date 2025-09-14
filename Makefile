CC = gcc
CFLAGS = -Wall -Wextra
OPT = -O0
BIN = bin
SRC = src
TARGET  = example
DBIN    = $(BIN)/debug
DTARGET = example_debug
OBJ = $(BIN)/ResultCodes.o $(BIN)/Server.o

all: $(TARGET)

# ------------------------
# Release build
# ------------------------
$(TARGET): $(BIN)/main.o $(OBJ)
	$(CC) $(CFLAGS) $(OPT) -o $@ $^

$(BIN)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

# ------------------------
# Debug build
# ------------------------
debug: $(DTARGET)

$(DTARGET): $(DBIN)/main.o
	$(CC) $(CFLAGS) -g -O0 -o $@ $^

$(DBIN)/%.o: $(SRC)/%.c | $(DBIN)
	$(CC) $(CFLAGS) -g -O0 -c $< -o $@

# ------------------------
# Ensure bin directories exist
# ------------------------
$(BIN):
	mkdir -p $(BIN)

$(DBIN):
	mkdir -p $(DBIN)

# ------------------------
# Cleanup
# ------------------------
clean:
	rm -rf $(BIN) $(TARGET) $(DTARGET)

.PHONY: all clean debug

