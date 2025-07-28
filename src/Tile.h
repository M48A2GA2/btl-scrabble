#ifndef TILE_H
#define TILE_H

class Tile
{
public:
    char letter;
    int points;
    bool isBlank;
    char usedAs;

    Tile(char l = ' ', int p = 0, bool blank = false);
    bool isEmpty() const;
};

#endif
