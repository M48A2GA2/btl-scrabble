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
    std::vector<std::unique_ptr<Tile>> tiles;  // Collection of tiles in the bag
    mutable std::mt19937 rng;                  // Random number generator for shuffling

    /**
     * Initialize the bag with the standard Scrabble tile distribution
     * Creates 100 tiles according to official Scrabble distribution
     */
    void initializeStandardTiles();

public:
    /**
     * Default constructor - initializes with random seed
     */
    TileBag();

    /**
     * Constructor with specific seed for reproducible randomness
     * @param seed The seed for the random number generator
     */
    explicit TileBag(unsigned int seed);

    // Move-only class (disable copy operations)
    TileBag(TileBag &&other) noexcept = default;
    TileBag &operator=(TileBag &&other) noexcept = default;
    TileBag(const TileBag &) = delete;
    TileBag &operator=(const TileBag &) = delete;

    /**
     * Draw a tile from the bag
     * @return A unique pointer to a tile, or nullptr if bag is empty
     */
    std::unique_ptr<Tile> drawTile();

    /**
     * Add a tile to the bag
     * @param tile The tile to add
     * @throws std::invalid_argument if tile is null
     */
    void addTile(std::unique_ptr<Tile> tile);

    /**
     * Add multiple tiles to the bag
     * @param tiles Vector of tiles to add
     */
    void addTiles(std::vector<std::unique_ptr<Tile>> tiles);

    /**
     * Check if the bag is empty
     * @return true if bag is empty, false otherwise
     */
    bool isEmpty() const noexcept { return tiles.empty(); }

    /**
     * Get the number of remaining tiles in the bag
     * @return The number of tiles in the bag
     */
    size_t remainingTiles() const noexcept { return tiles.size(); }

    /**
     * Shuffle the tiles in the bag
     * Uses the internal random number generator
     */
    void shuffle();
};

#endif
