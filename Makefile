CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
OBJ_DIR = obj
SRC_DIR = src
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/bank.c $(SRC_DIR)/process.c $(SRC_DIR)/sleep-util.c 
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
TARGET = $(BIN_DIR)/main

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Clean object files and executable
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the program
run: $(TARGET)
	$(TARGET) 10 5 7 maximum.txt # Or your input file name.
