#include "Player.h"
#include <algorithm>
#include <stdexcept>

Player::Player(std::string playerName)
    : name(std::move(playerName)), score(0)
{
    hand.reserve(HAND_SIZE);
}

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

int Player::getHandValue() const noexcept
{
    int totalValue = 0;
    for (const auto &tile : hand)
    {
        totalValue += tile->points;
    }
    return totalValue;
}

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

bool Player::hasTile(char letter) const noexcept
{
    return findTileIndex(letter) != -1;
}
