#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"
#include <vector>
#include <memory>
#include <utility>
#include <string>

class Board
{
public:
    static const int BOARD_SIZE = 15;

    enum class Premium
    {
        NONE,
        DOUBLE_LETTER,
        TRIPLE_LETTER,
        DOUBLE_WORD,
        TRIPLE_WORD,
        START
    };

    struct WordInfo
    {
        std::string word;
        std::vector<std::pair<int, int>> positions;
        int score;
        bool isMainWord;

        WordInfo() : score(0), isMainWord(false) {}
    };

    struct Square
    {
        std::unique_ptr<Tile> tile;
        Premium premium;

        explicit Square(Premium p = Premium::NONE) : premium(p) {}

        // Disable copy constructor and assignment
        Square(const Square &) = delete;
        Square &operator=(const Square &) = delete;

        // Enable move constructor and assignment
        Square(Square &&) = default;
        Square &operator=(Square &&) = default;

        bool isOccupied() const noexcept { return tile != nullptr; }
    };

private:
    std::vector<std::vector<Square>> grid;
    bool firstMovePlayed;

    // Helper methods for initializing premium squares
    void initializeTripleWordSquares();
    void initializeDoubleWordSquares();
    void initializeTripleLetterSquares();
    void initializeDoubleLetterSquares();

public:
    Board();

    // Board initialization
    void initializePremiumSquares();

    // Board state queries
    bool isInBounds(int row, int col) const noexcept;
    const Square &getSquare(int row, int col) const;
    Square &getSquare(int row, int col);
    Premium getPremium(int row, int col) const;

    // Tile placement
    bool placeTile(int row, int col, std::unique_ptr<Tile> tile);
    bool canPlaceTile(int row, int col) const noexcept;

    // Move validation
    bool isFirstMovePlayed() const noexcept { return firstMovePlayed; }
    bool isValidFirstMove(int row, int col) const noexcept;
    bool isAdjacentToExistingTile(int row, int col) const noexcept;
    bool isEmpty() const noexcept;

    // Word formation and scoring
    std::vector<WordInfo> getWordsFormedByMove(
        const std::vector<std::pair<int, int>> &positions) const;
    std::string getWordInDirection(int row, int col, int deltaRow, int deltaCol) const;
    int calculateWordScore(const WordInfo &wordInfo) const;
};

#endif
