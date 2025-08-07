#ifndef PLAYER_H
#define PLAYER_H

#include "Tile.h"
#include <vector>
#include <memory>
#include <string>

class Player
{
private:
    std::string name;
    int score;
    std::vector<std::unique_ptr<Tile>> hand;
    static constexpr int HAND_SIZE = 7;

public:
    explicit Player(std::string playerName);

    // Move-only class
    Player(Player &&other) noexcept = default;
    Player &operator=(Player &&other) noexcept = default;
    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;

    // Tile management
    void addTile(std::unique_ptr<Tile> tile);
    std::unique_ptr<Tile> removeTile(size_t index);
    bool hasEmptySlots() const noexcept { return hand.size() < HAND_SIZE; }

    // Getters
    int getScore() const noexcept { return score; }
    const std::string &getName() const noexcept { return name; }
    const std::vector<std::unique_ptr<Tile>> &getHand() const noexcept { return hand; }
    size_t getHandSize() const noexcept { return hand.size(); }

    // Score management
    void addScore(int points) noexcept { score += points; }
    void subtractScore(int points) noexcept { score -= points; }

    // Get total points value of tiles in hand
    int getHandValue() const noexcept;

    // Find tile index by letter
    int findTileIndex(char letter) const noexcept;

    // Check if player has a specific tile
    bool hasTile(char letter) const noexcept;
};

#endif
