#include "Player.h"
#include <algorithm>
#include <stdexcept>

/**
 * Constructor for Player class
 * 
 * @param playerName The name of the player
 */
Player::Player(std::string playerName)
    : name(std::move(playerName)), score(0)
{
    hand.reserve(HAND_SIZE);
}

/**
 * Add a tile to the player's hand
 * 
 * @param tile The tile to add
 * @throws std::invalid_argument if tile is null
 * @throws std::runtime_error if hand is full
 */
void Player::addTile(std::unique_ptr<Tile> tile)
{
    if (!tile)
    {
        throw std::invalid_argument("Cannot add null tile");
    }

    if (hand.size() >= HAND_SIZE)
    {
        throw std::runtime_error("Hand is full, cannot add more tiles");
    }

    hand.push_back(std::move(tile));
}

/**
 * Remove a tile from the player's hand
 * 
 * @param index The index of the tile to remove
 * @return The removed tile, or nullptr if index is invalid
 */
std::unique_ptr<Tile> Player::removeTile(size_t index)
{
    if (index >= hand.size())
    {
        return nullptr;
    }

    auto tile = std::move(hand[index]);
    hand.erase(hand.begin() + index);
    return tile;
}

/**
 * Calculate the total point value of tiles in hand
 * 
 * @return The total value of tiles in hand
 */
int Player::getHandValue() const noexcept
{
    int totalValue = 0;
    for (const auto &tile : hand)
    {
        totalValue += tile->points;
    }
    return totalValue;
}

/**
 * Find the index of a tile with a specific letter
 * 
 * @param letter The letter to search for
 * @return The index of the tile, or -1 if not found
 */
int Player::findTileIndex(char letter) const noexcept
{
    for (size_t i = 0; i < hand.size(); ++i)
    {
        if (hand[i]->letter == letter)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

/**
 * Check if the player has a tile with a specific letter
 * 
 * @param letter The letter to search for
 * @return true if the player has the tile, false otherwise
 */
bool Player::hasTile(char letter) const noexcept
{
    return findTileIndex(letter) != -1;
}
