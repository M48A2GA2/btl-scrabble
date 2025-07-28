#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"
#include <vector>
#include <memory>

class Board
{
public:
    static const int BOARD_SIZE = 15;

    enum Premium
    {
        NONE,
        DOUBLE_LETTER,
        TRIPLE_LETTER,
        DOUBLE_WORD,
        TRIPLE_WORD,
        START
    };

    struct Square
    {
        std::unique_ptr<Tile> tile;
        Premium premium;

        Square(Premium p = NONE) : premium(p) {}

        Square(const Square &) = delete;
        Square &operator=(const Square &) = delete;
        Square(Square &&) = default;
        Square &operator=(Square &&) = default;

        bool isOccupied() const { return tile != nullptr; }
    };

private:
    std::vector<std::vector<Square>> grid;

public:
    Board();
    void initializePremiumSquares();
    bool isInBounds(int row, int col) const;
    Square &getSquare(int row, int col);
    const Square &getSquare(int row, int col) const;
    bool placeTile(int row, int col, std::unique_ptr<Tile> tile);
};

#endif
