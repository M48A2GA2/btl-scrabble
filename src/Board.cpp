#include "Board.h"

Board::Board() 
{
    grid.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        grid[i].resize(BOARD_SIZE);
    }
    initializePremiumSquares();
}

void Board::initializePremiumSquares()
{
    // Set center square as START
    grid[7][7] = Square(START);

    // Triple Word squares
    int tw_positions[][2] = {{0, 0}, {0, 7}, {0, 14}, {7, 0}, {7, 14}, {14, 0}, {14, 7}, {14, 14}};
    for (auto pos : tw_positions)
    {
        grid[pos[0]][pos[1]] = Square(TRIPLE_WORD);
    }

    // Double Word squares (simplified pattern)
    for (int i = 1; i < 14; i++)
    {
        if (i != 7)
        {
            grid[i][i] = Square(DOUBLE_WORD);
            grid[i][14 - i] = Square(DOUBLE_WORD);
        }
    }

    // Add more premium squares as needed...
}

bool Board::isInBounds(int row, int col) const
{
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

Board::Square &Board::getSquare(int row, int col)
{
    return grid[row][col];
}

const Board::Square &Board::getSquare(int row, int col) const
{
    return grid[row][col];
}

bool Board::placeTile(int row, int col, std::unique_ptr<Tile> tile)
{
    if (!isInBounds(row, col) || grid[row][col].isOccupied())
    {
        return false;
    }
    grid[row][col].tile = std::move(tile);
    return true;
}
