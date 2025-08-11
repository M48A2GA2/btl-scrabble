#include "TileBag.h"
#include <algorithm>
#include <random>

TileBag::TileBag() : rng(std::random_device{}())
{
    initializeStandardTiles();
}

TileBag::TileBag(unsigned int seed) : rng(seed)
{
    initializeStandardTiles();
}

void TileBag::initializeStandardTiles()
{
    // Standard English Scrabble tile distribution
    struct TileInfo
    {
        char letter;
        int count;
        int points;
    };

    constexpr TileInfo tileData[] = {
        {'A', 9, 1}, {'B', 2, 3}, {'C', 2, 3}, {'D', 4, 2}, {'E', 12, 1}, {'F', 2, 4}, {'G', 3, 2}, {'H', 2, 4}, {'I', 9, 1}, {'J', 1, 8}, {'K', 1, 5}, {'L', 4, 1}, {'M', 2, 3}, {'N', 6, 1}, {'O', 8, 1}, {'P', 2, 3}, {'Q', 1, 10}, {'R', 6, 1}, {'S', 4, 1}, {'T', 6, 1}, {'U', 4, 1}, {'V', 2, 4}, {'W', 2, 4}, {'X', 1, 8}, {'Y', 2, 4}, {'Z', 1, 10}, {' ', 2, 0} // Blank tiles
    };

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

void TileBag::shuffle()
{
    std::shuffle(tiles.begin(), tiles.end(), rng);
}
