# Makefile for Tractor ECU

CC = gcc
CFLAGS = -Wall -Wextra -I./src
LDFLAGS = -lm
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/ecu_controller

# Find all .c files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/engine/engine_control.c \
          $(SRC_DIR)/hydraulics/hydraulics.c \
          $(SRC_DIR)/transmission/transmission.c \
          $(SRC_DIR)/diagnostics/diagnostics.c \
          $(SRC_DIR)/canbus/canbus.c \
          $(SRC_DIR)/pto/pto.c \
          $(SRC_DIR)/telematics/telematics.c \
          $(SRC_DIR)/implement/implement.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/engine
	mkdir -p $(BUILD_DIR)/hydraulics
	mkdir -p $(BUILD_DIR)/transmission
	mkdir -p $(BUILD_DIR)/diagnostics
	mkdir -p $(BUILD_DIR)/canbus
	mkdir -p $(BUILD_DIR)/pto
	mkdir -p $(BUILD_DIR)/telematics
	mkdir -p $(BUILD_DIR)/implement

# Link the executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program in demo mode
demo: $(TARGET)
	./$(TARGET) --demo

# Run the program in continuous mode
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

# Rebuild from scratch
rebuild: clean all

# Show help
help:
	@echo "Tractor ECU Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the ECU controller (default)"
	@echo "  demo     - Build and run in demo mode"
	@echo "  run      - Build and run in continuous mode"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and build from scratch"
	@echo "  help     - Show this help message"

.PHONY: all demo run clean rebuild help
