# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -Wunused-parameter

# Source files
SRC = main.c

# Output executable
TARGET = my_program

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lusb-1.0

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up build artifacts
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean run