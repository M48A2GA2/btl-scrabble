#ifndef TILE_H
#define TILE_H

class Tile
{
public:
    char letter;      // The letter on the tile (' ' for blank tiles)
    int points;       // Point value of the tile
    bool isBlank;     // Whether this is a blank tile
    char usedAs;      // What letter a blank tile represents

    /**
     * Constructor for Tile
     * @param l The letter on the tile (space for blank)
     * @param p Point value of the tile
     * @param blank Whether this is a blank tile
     */
    explicit Tile(char l = ' ', int p = 0, bool blank = false);

    // Move constructor and assignment (defaulted for efficiency)
    Tile(Tile &&other) noexcept = default;
    Tile &operator=(Tile &&other) noexcept = default;

    // Delete copy constructor and assignment to prevent accidental copying
    Tile(const Tile &) = delete;
    Tile &operator=(const Tile &) = delete;

    /**
     * Check if the tile is a blank tile
     * @return true if the tile is blank, false otherwise
     */
    bool isEmpty() const noexcept;

    /**
     * Get the display letter for the tile
     * For blank tiles, returns the letter it's being used as
     * @return The display letter
     */
    char getDisplayLetter() const noexcept;

    /**
     * Set what letter a blank tile represents
     * Only works on blank tiles
     * @param c The letter to represent
     */
    void setBlankAs(char c) noexcept;
};

#endif
