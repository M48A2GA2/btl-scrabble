CXX = g++
CXXFLAGS = -g -Wall -std=c++17
LIBS = -lSDL2 -lSDL2_ttf

TARGET = scrabble
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean
