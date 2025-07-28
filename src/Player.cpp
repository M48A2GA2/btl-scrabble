#include "Player.h"

Player::Player(const std::string &playerName) : name(playerName), score(0) {}

void Player::addTile(std::unique_ptr<Tile> tile)
{
    if (hand.size() < HAND_SIZE)
    {
        hand.push_back(std::move(tile));
    }
}

bool Player::hasEmptySlots() const
{
    return hand.size() < HAND_SIZE;
}

std::unique_ptr<Tile> Player::removeTile(int index)
{
    if (index < 0 || static_cast<size_t>(index) >= hand.size())
    {
        return nullptr;
    }

    auto tile = std::move(hand[index]);
    hand.erase(hand.begin() + index);
    return tile;
}
