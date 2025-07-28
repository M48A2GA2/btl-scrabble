#ifndef TILE_BAG_H
#define TILE_BAG_H

#include "Tile.h"
#include <vector>
#include <memory>
#include <random>

class TileBag
{
private:
    std::vector<std::unique_ptr<Tile>> tiles;
    std::mt19937 rng;

public:
    TileBag();
    void initializeStandardTiles();
    std::unique_ptr<Tile> drawTile();
    bool isEmpty() const;
    int remainingTiles() const;
};

#endif
