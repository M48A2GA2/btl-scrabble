#include "Tile.h"

Tile::Tile(char l, int p, bool blank)
    : letter(l), points(p), isBlank(blank), usedAs(l) {}

bool Tile::isEmpty() const
{
    return letter == ' ';
}
