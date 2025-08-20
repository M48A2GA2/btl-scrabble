#ifndef PLAYER_H
#define PLAYER_H

#include "Tile.h"
#include <vector>
#include <memory>
#include <string>

class Player
{
private:
    std::string name;                              // Player's name
    int score;                                     // Player's current score
    std::vector<std::unique_ptr<Tile>> hand;       // Player's tiles (max 7)
    static constexpr int HAND_SIZE = 7;            // Standard Scrabble hand size

public:
    /**
     * Constructor for Player
     * @param playerName The name of the player
     */
    explicit Player(std::string playerName);

    // Move-only class (disable copy operations)
    Player(Player &&other) noexcept = default;
    Player &operator=(Player &&other) noexcept = default;
    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;

    // Tile management
    /**
     * Add a tile to the player's hand
     * @param tile The tile to add
     * @throws std::invalid_argument if tile is null
     * @throws std::runtime_error if hand is full
     */
    void addTile(std::unique_ptr<Tile> tile);

    /**
     * Remove a tile from the player's hand
     * @param index The index of the tile to remove
     * @return The removed tile, or nullptr if index is invalid
     */
    std::unique_ptr<Tile> removeTile(size_t index);

    /**
     * Check if the player's hand has empty slots
     * @return true if hand has fewer than HAND_SIZE tiles, false otherwise
     */
    bool hasEmptySlots() const noexcept { return hand.size() < HAND_SIZE; }

    // Getters
    /**
     * Get the player's current score
     * @return The player's score
     */
    int getScore() const noexcept { return score; }

    /**
     * Get the player's name
     * @return The player's name
     */
    const std::string &getName() const noexcept { return name; }

    /**
     * Get the player's hand (tiles)
     * @return Const reference to the hand vector
     */
    const std::vector<std::unique_ptr<Tile>> &getHand() const noexcept { return hand; }

    /**
     * Get the number of tiles in the player's hand
     * @return The number of tiles
     */
    size_t getHandSize() const noexcept { return hand.size(); }

    // Score management
    /**
     * Add points to the player's score
     * @param points The points to add
     */
    void addScore(int points) noexcept { score += points; }

    /**
     * Subtract points from the player's score
     * @param points The points to subtract
     */
    void subtractScore(int points) noexcept { score -= points; }

    // Hand analysis
    /**
     * Calculate the total point value of tiles in hand
     * @return The total value of tiles in hand
     */
    int getHandValue() const noexcept;

    /**
     * Find the index of a tile with a specific letter
     * @param letter The letter to search for
     * @return The index of the tile, or -1 if not found
     */
    int findTileIndex(char letter) const noexcept;

    /**
     * Check if the player has a tile with a specific letter
     * @param letter The letter to search for
     * @return true if the player has the tile, false otherwise
     */
    bool hasTile(char letter) const noexcept;
};

#endif
