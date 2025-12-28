CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Isrc -Iassembler

# =======================
# targets
# =======================
EMU_TARGET  := asp16
ASM_TARGET  := asmp

# =======================
# emulator build
# =======================
SRC_DIR     := src
BUILD_DIR   := build

EMU_SOURCES := $(shell find $(SRC_DIR) -name "*.cpp")
EMU_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(EMU_SOURCES))

# =======================
# assembler build
# =======================
ASM_SRC     := assembler/asmp.cpp
ASM_OBJ     := build/asmp.o

# =======================
# default
# =======================
all: $(EMU_TARGET) $(ASM_TARGET)

# =======================
# emulator
# =======================
$(EMU_TARGET): $(EMU_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =======================
# assembler
# =======================
$(ASM_TARGET): $(ASM_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(ASM_OBJ): $(ASM_SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(EMU_TARGET) $(ASM_TARGET)