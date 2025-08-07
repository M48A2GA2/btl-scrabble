#ifndef TILE_H
#define TILE_H

class Tile
{
public:
    char letter;
    int points;
    bool isBlank;
    char usedAs; // For blank tiles

    // Constructor declaration only
    explicit Tile(char l = ' ', int p = 0, bool blank = false);

    // Move constructor and assignment
    Tile(Tile &&other) noexcept = default;
    Tile &operator=(Tile &&other) noexcept = default;

    // Delete copy constructor and assignment to prevent accidental copying
    Tile(const Tile &) = delete;
    Tile &operator=(const Tile &) = delete;

    // Function declarations only
    bool isEmpty() const noexcept;
    char getDisplayLetter() const noexcept;
    void setBlankAs(char c) noexcept;
};

#endif
