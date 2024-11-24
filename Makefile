CXX = g++
CXXFLAGS = -std=c++20 -Wall -Iinclude
BUILD_DIR = build
TARGET = $(BUILD_DIR)/delta-demo
SOURCES = src/delta.cc src/main.cc src/nfa.cc
OBJECTS = $(SOURCES:src/%.cc=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
