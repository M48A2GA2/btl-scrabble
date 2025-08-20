#include "Tile.h"

/**
 * Constructor for Tile class
 * 
 * @param l The letter on the tile (space for blank tiles)
 * @param p Point value of the tile
 * @param blank Whether this is a blank tile
 */
Tile::Tile(char l, int p, bool blank)
    : letter(l), points(p), isBlank(blank), usedAs(l) {}

/**
 * Check if the tile is a blank tile
 * 
 * @return true if the tile is blank (letter is space), false otherwise
 */
bool Tile::isEmpty() const noexcept
{
    return letter == ' ';
}

/**
 * Get the display letter for the tile
 * For blank tiles, returns the letter it's being used as
 * 
 * @return The display letter
 */
char Tile::getDisplayLetter() const noexcept
{
    return isBlank ? usedAs : letter;
}

/**
 * Set what letter a blank tile represents
 * Only works on blank tiles
 * 
 * @param c The letter to represent
 */
void Tile::setBlankAs(char c) noexcept
{
    if (isBlank)
    {
        usedAs = c;
    }
}
