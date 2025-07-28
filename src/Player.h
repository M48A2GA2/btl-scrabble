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
    static const int HAND_SIZE = 7;

public:
    Player(const std::string &playerName);

    void addTile(std::unique_ptr<Tile> tile);
    bool hasEmptySlots() const;
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    const std::string &getName() const { return name; }
    const std::vector<std::unique_ptr<Tile>> &getHand() const { return hand; }

    // Remove tile at index (for placing on board)
    std::unique_ptr<Tile> removeTile(int index);
};

#endif
