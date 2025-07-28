#include "TileBag.h"
#include <algorithm>

TileBag::TileBag() : rng(std::random_device{}())
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
    TileInfo tileData[] = {
        {'A', 9, 1}, {'B', 2, 3}, {'C', 2, 3}, {'D', 4, 2}, {'E', 12, 1}, {'F', 2, 4}, {'G', 3, 2}, {'H', 2, 4}, {'I', 9, 1}, {'J', 1, 8}, {'K', 1, 5}, {'L', 4, 1}, {'M', 2, 3}, {'N', 6, 1}, {'O', 8, 1}, {'P', 2, 3}, {'Q', 1, 10}, {'R', 6, 1}, {'S', 4, 1}, {'T', 6, 1}, {'U', 4, 1}, {'V', 2, 4}, {'W', 2, 4}, {'X', 1, 8}, {'Y', 2, 4}, {'Z', 1, 10}, {' ', 2, 0} // Blank tiles
    };

    for (const auto &info : tileData)
    {
        for (int i = 0; i < info.count; i++)
        {
            tiles.push_back(std::make_unique<Tile>(info.letter, info.points, info.letter == ' '));
        }
    }

    // Shuffle the tiles
    std::shuffle(tiles.begin(), tiles.end(), rng);
}

std::unique_ptr<Tile> TileBag::drawTile()
{
    if (tiles.empty())
        return nullptr;

    auto tile = std::move(tiles.back());
    tiles.pop_back();
    return tile;
}

bool TileBag::isEmpty() const
{
    return tiles.empty();
}

int TileBag::remainingTiles() const
{
    return tiles.size();
}
