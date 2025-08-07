#include "Tile.h"

Tile::Tile(char l, int p, bool blank)
    : letter(l), points(p), isBlank(blank), usedAs(l) {}

bool Tile::isEmpty() const noexcept
{
    return letter == ' ';
}

char Tile::getDisplayLetter() const noexcept
{
    return isBlank ? usedAs : letter;
}

void Tile::setBlankAs(char c) noexcept
{
    if (isBlank)
    {
        usedAs = c;
    }
}
