# Makefile for Scrabble Game
CXX = g++
CXXFLAGS = -g -Wall -std=c++17 -O2
TARGET = scrabble
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS = -lSDL2 -lSDL2_ttf
endif
ifeq ($(UNAME_S),Darwin)
    LIBS = -lSDL2 -lSDL2_ttf
    CXXFLAGS += -I/usr/local/include
    LDFLAGS += -L/usr/local/lib
endif
ifdef OS
   ifeq ($(OS),Windows_NT)
       TARGET = scrabble.exe
       LIBS = -lSDL2 -lSDL2_ttf -lSDL2main
       CXXFLAGS += -Dmain=SDL_main
   endif
endif

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo "Build complete! Run ./$(TARGET) to play."

# Individual object files (for incremental compilation)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) $(OBJECTS)
	@echo "Clean complete."

# Install dependencies (Linux/Ubuntu)
install-deps-ubuntu:
	sudo apt update
	sudo apt install -y build-essential libsdl2-dev libsdl2-ttf-dev

# Install dependencies (macOS with Homebrew)
install-deps-macos:
	brew install sdl2 sdl2_ttf

# Run the game
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the game (default)"
	@echo "  clean        - Remove build files"
	@echo "  run          - Build and run the game"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  install-deps-ubuntu - Install dependencies on Ubuntu"
	@echo "  install-deps-macos  - Install dependencies on macOS"
	@echo "  help         - Show this help message"

.PHONY: all clean run debug release help install-deps-ubuntu install-deps-macos
