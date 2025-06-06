# Usage: make VERSION=1.2.3

# Variables
CC = gcc
APP_NAME = Shodrone-Drone-App
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/objects
VERSION ?= 0.0.1
TARGET = $(BIN_DIR)/$(APP_NAME)_$(VERSION)
CFLAGS = -Wall -Wextra -pthread -lrt -DAPP_VERSION="\"$(VERSION)\"" -DAPP_NAME="\"$(APP_NAME)\""

# Source and object files
SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile .c to .o into bin/objects/ folder
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the binary
run: all
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BIN_DIR)

.PHONY: all run clean
