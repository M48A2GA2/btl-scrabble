#ifndef TILE_BAG_H
#define TILE_BAG_H

#include "Tile.h"
#include <vector>
#include <memory>
#include <random>
#include <stdexcept>

class TileBag
{
private:
    std::vector<std::unique_ptr<Tile>> tiles;
    mutable std::mt19937 rng;

    void initializeStandardTiles();

public:
    TileBag();
    explicit TileBag(unsigned int seed);

    // Move-only class
    TileBag(TileBag &&other) noexcept = default;
    TileBag &operator=(TileBag &&other) noexcept = default;
    TileBag(const TileBag &) = delete;
    TileBag &operator=(const TileBag &) = delete;

    std::unique_ptr<Tile> drawTile();
    void addTile(std::unique_ptr<Tile> tile);
    void addTiles(std::vector<std::unique_ptr<Tile>> tiles);

    bool isEmpty() const noexcept { return tiles.empty(); }
    size_t remainingTiles() const noexcept { return tiles.size(); }

    // Shuffle the bag
    void shuffle();
};

#endif
