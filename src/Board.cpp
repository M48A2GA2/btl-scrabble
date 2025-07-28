#include "Board.h"

Board::Board() : firstMovePlayed(false)
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
// {
//     if (!isInBounds(row, col) || grid[row][col].isOccupied())
//     {
//         return false;
//     }
//     grid[row][col].tile = std::move(tile);
//     return true;
// }
{
    if (!canPlaceTile(row, col))
    {
        return false;
    }

    grid[row][col].tile = std::move(tile);

    // Mark first move as played if this was on the center square
    if (row == 7 && col == 7)
    {
        firstMovePlayed = true;
    }

    return true;
}

bool Board::canPlaceTile(int row, int col) const
{
    if (!isInBounds(row, col) || grid[row][col].isOccupied())
    {
        return false;
    }

    // First move must be on center square
    if (!firstMovePlayed)
    {
        return isValidFirstMove(row, col);
    }

    // Subsequent moves must be adjacent to existing tiles
    return isAdjacentToExistingTile(row, col);
}

bool Board::isValidFirstMove(int row, int col) const
{
    return row == 7 && col == 7; // Center square
}

bool Board::isAdjacentToExistingTile(int row, int col) const
{
    // Check all four directions
    int directions[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (auto dir : directions)
    {
        int newRow = row + dir[0];
        int newCol = col + dir[1];

        if (isInBounds(newRow, newCol) && grid[newRow][newCol].isOccupied())
        {
            return true;
        }
    }

    return false;
}

std::vector<std::string> Board::getWordsFormedByMove(int row, int col) const
{
    std::vector<std::string> words;

    // Get horizontal word
    std::string horizontalWord = getWordInDirection(row, col, 0, 1);
    if (horizontalWord.length() > 1)
    {
        words.push_back(horizontalWord);
    }

    // Get vertical word
    std::string verticalWord = getWordInDirection(row, col, 1, 0);
    if (verticalWord.length() > 1)
    {
        words.push_back(verticalWord);
    }

    return words;
}

std::string Board::getWordInDirection(int row, int col, int deltaRow, int deltaCol) const
{
    // Find start of word
    int startRow = row, startCol = col;
    while (isInBounds(startRow - deltaRow, startCol - deltaCol) &&
           grid[startRow - deltaRow][startCol - deltaCol].isOccupied())
    {
        startRow -= deltaRow;
        startCol -= deltaCol;
    }

    // Build word from start
    std::string word;
    int currentRow = startRow, currentCol = startCol;

    while (isInBounds(currentRow, currentCol) && grid[currentRow][currentCol].isOccupied())
    {
        char letter = grid[currentRow][currentCol].tile->letter;
        if (letter == ' ')
        {
            // Handle blank tiles - for now, just use '?'
            letter = '?';
        }
        word += letter;
        currentRow += deltaRow;
        currentCol += deltaCol;
    }

    return word;
}