CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
# Dependency generation flags
DEPFLAGS = -MMD -MP
LDFLAGS = -lSDL2

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Locate all .cpp files in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Map .cpp files to .o object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Map .o object files to .d dependency files
DEPS = $(OBJS:.o=.d)

# The final executable name
TARGET = chip8.out

# Default target
all: $(TARGET)

# Link all object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Compile each source file into an object file and generate .d files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Create the build directory if it does not exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Remove all compiled files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Include the generated dependency files
-include $(DEPS)

.PHONY: all clean