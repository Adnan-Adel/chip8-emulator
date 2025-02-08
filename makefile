CPP = g++
CPPFLAGS = -Wall -Wextra -std=c++17 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

TARGET = chip8-emulator
DEBUG_TARGET = chip8-emulator-debug

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

debug: CPPFLAGS += -DDEBUG -g
debug: LDFLAGS += -g
debug: $(OBJ_FILES)
	$(CPP) $(CPPFLAGS) -o $(DEBUG_TARGET) $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CPP) $(CPPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(DEBUG_TARGET)

.PHONY: all clean debug

