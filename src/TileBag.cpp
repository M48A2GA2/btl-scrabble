#include "TileBag.h"
#include <algorithm>
#include <random>
#include <array>

/**
 * Default constructor - initializes with random seed
 */
TileBag::TileBag() : rng(std::random_device{}())
{
    initializeStandardTiles();
}

/**
 * Constructor with specific seed for reproducible randomness
 * 
 * @param seed The seed for the random number generator
 */
TileBag::TileBag(unsigned int seed) : rng(seed)
{
    initializeStandardTiles();
}

/**
 * Initialize the bag with the standard Scrabble tile distribution
 * Creates 100 tiles according to official Scrabble distribution
 */
void TileBag::initializeStandardTiles()
{
    // Standard English Scrabble tile distribution
    struct TileInfo
    {
        char letter;
        int count;
        int points;
    };

    // Standard Scrabble tile distribution
    constexpr std::array<TileInfo, 27> tileData = {{
        {'A', 9, 1}, {'B', 2, 3}, {'C', 2, 3}, {'D', 4, 2}, {'E', 12, 1}, 
        {'F', 2, 4}, {'G', 3, 2}, {'H', 2, 4}, {'I', 9, 1}, {'J', 1, 8}, 
        {'K', 1, 5}, {'L', 4, 1}, {'M', 2, 3}, {'N', 6, 1}, {'O', 8, 1}, 
        {'P', 2, 3}, {'Q', 1, 10}, {'R', 6, 1}, {'S', 4, 1}, {'T', 6, 1}, 
        {'U', 4, 1}, {'V', 2, 4}, {'W', 2, 4}, {'X', 1, 8}, {'Y', 2, 4}, 
        {'Z', 1, 10}, {' ', 2, 0} // Blank tiles
    }};

    tiles.reserve(100); // Standard Scrabble has 100 tiles

    for (const auto &info : tileData)
    {
        for (int i = 0; i < info.count; ++i)
        {
            tiles.push_back(std::make_unique<Tile>(
                info.letter, info.points, info.letter == ' '));
        }
    }

    shuffle();
}

/**
 * Draw a tile from the bag
 * 
 * @return A unique pointer to a tile, or nullptr if bag is empty
 */
std::unique_ptr<Tile> TileBag::drawTile()
{
    if (tiles.empty())
    {
        return nullptr;
    }

    auto tile = std::move(tiles.back());
    tiles.pop_back();
    return tile;
}

/**
 * Add a tile to the bag
 * 
 * @param tile The tile to add
 * @throws std::invalid_argument if tile is null
 */
void TileBag::addTile(std::unique_ptr<Tile> tile)
{
    if (!tile)
    {
        throw std::invalid_argument("Cannot add null tile to bag");
    }

    // Insert at random position to maintain randomness
    std::uniform_int_distribution<size_t> dist(0, tiles.size());
    auto pos = tiles.begin() + dist(rng);
    tiles.insert(pos, std::move(tile));
}

/**
 * Add multiple tiles to the bag
 * 
 * @param newTiles Vector of tiles to add
 */
void TileBag::addTiles(std::vector<std::unique_ptr<Tile>> newTiles)
{
    for (auto &tile : newTiles)
    {
        if (tile)
        {
            addTile(std::move(tile));
        }
    }
}

/**
 * Shuffle the tiles in the bag
 * Uses the internal random number generator
 */
void TileBag::shuffle()
{
    std::shuffle(tiles.begin(), tiles.end(), rng);
}
