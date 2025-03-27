CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O3
LDFLAGS := 

SRC_DIR := src
APP_DIR := app
BUILD_DIR := build

SRC_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
APP_SOURCES := $(wildcard $(APP_DIR)/*.cpp)

SRC_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_SOURCES))
APP_OBJECTS := $(patsubst $(APP_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(APP_SOURCES))

OBJECTS := $(SRC_OBJECTS) $(APP_OBJECTS)

EXECUTABLE := program

ejecutar: all
	./program

all: $(BUILD_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(APP_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)

.PHONY: all clean ejecutar