CXX = g++
CXXFLAGS = -g -Wall -std=c++17
LIBS = -lSDL2 -lSDL2_ttf

# Main target
TARGET = scrabble
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Test target
TEST_TARGET = test_dict
TEST_SOURCES = test_dictionary.cpp src/Dictionary.cpp src/Tile.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build main game
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

# Build test program
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

# Compile source files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

# Compile test files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Run tests
run-test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean run run-test test