#include "Board.h"
#include <array>

Board::Board() : firstMovePlayed(false)
{
    grid.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        grid[i].resize(BOARD_SIZE);
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            grid[i][j] = Square(Board::Premium::NONE);
        }
    }
    initializePremiumSquares();
}

void Board::initializePremiumSquares()
{
    // Initialize all squares as NONE first
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            grid[r][c] = Square(Board::Premium::NONE);
        }
    }

    // Set center square as START
    grid[7][7] = Square(Board::Premium::START);

    // Initialize premium squares by type
    initializeTripleWordSquares();
    initializeDoubleWordSquares();
    initializeTripleLetterSquares();
    initializeDoubleLetterSquares();
}

void Board::initializeTripleWordSquares()
{
    const std::array<std::pair<int, int>, 8> positions = {{
        {0, 0}, {0, 7}, {0, 14}, 
        {7, 0}, {7, 14}, 
        {14, 0}, {14, 7}, {14, 14}
    }};
    
    for (const auto& pos : positions)
    {
        grid[pos.first][pos.second] = Square(Board::Premium::TRIPLE_WORD);
    }
}

void Board::initializeDoubleWordSquares()
{
    // Diagonal squares excluding center
    for (int i = 1; i < 14; i++)
    {
        if (i != 7)
        {
            grid[i][i] = Square(Board::Premium::DOUBLE_WORD);
            grid[i][14 - i] = Square(Board::Premium::DOUBLE_WORD);
        }
    }
}

void Board::initializeTripleLetterSquares()
{
    const std::array<std::pair<int, int>, 12> positions = {{
        {1, 5}, {1, 9}, 
        {5, 1}, {5, 5}, {5, 9}, {5, 13},
        {9, 1}, {9, 5}, {9, 9}, {9, 13}, 
        {13, 5}, {13, 9}
    }};
    
    for (const auto& pos : positions)
    {
        grid[pos.first][pos.second] = Square(Board::Premium::TRIPLE_LETTER);
    }
}

void Board::initializeDoubleLetterSquares()
{
    const std::array<std::pair<int, int>, 24> positions = {{
        {0, 3}, {0, 11}, 
        {2, 6}, {2, 8}, 
        {3, 0}, {3, 7}, {3, 14},
        {6, 2}, {6, 6}, {6, 8}, {6, 12}, 
        {7, 3}, {7, 11},
        {8, 2}, {8, 6}, {8, 8}, {8, 12}, 
        {11, 0}, {11, 7}, {11, 14},
        {12, 6}, {12, 8}, 
        {14, 3}, {14, 11}
    }};
    
    for (const auto& pos : positions)
    {
        grid[pos.first][pos.second] = Square(Board::Premium::DOUBLE_LETTER);
    }
}

bool Board::isInBounds(int row, int col) const noexcept
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

Board::Premium Board::getPremium(int row, int col) const
{
    if (!isInBounds(row, col))
        return Board::Premium::NONE;
    return grid[row][col].premium;
}

bool Board::placeTile(int row, int col, std::unique_ptr<Tile> tile)
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

    // Consume premium so it applies only once
    grid[row][col].premium = Board::Premium::NONE;

    return true;
}

bool Board::canPlaceTile(int row, int col) const noexcept
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

bool Board::isValidFirstMove(int row, int col) const noexcept
{
    return row == 7 && col == 7; // Center square
}

bool Board::isAdjacentToExistingTile(int row, int col) const noexcept
{
    // Check all four directions: up, down, left, right
    const std::array<std::pair<int, int>, 4> directions = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    for (const auto &dir : directions)
    {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (isInBounds(newRow, newCol) && grid[newRow][newCol].isOccupied())
        {
            return true;
        }
    }
    return false;
}

bool Board::isEmpty() const noexcept
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            if (grid[row][col].isOccupied())
            {
                return false;
            }
        }
    }
    return true;
}

std::vector<Board::WordInfo> Board::getWordsFormedByMove(
    const std::vector<std::pair<int, int>> &positions) const
{
    std::vector<WordInfo> words;

    if (positions.empty())
        return words;

    // For now, create simple word info from positions
    for (const auto &pos : positions)
    {
        int row = pos.first;
        int col = pos.second;

        // Get horizontal word
        std::string horizontalWord = getWordInDirection(row, col, 0, 1);
        if (horizontalWord.length() > 1)
        {
            WordInfo info;
            info.word = horizontalWord;
            info.score = 0; // Calculate later
            info.isMainWord = true;
            words.push_back(info);
        }

        // Get vertical word
        std::string verticalWord = getWordInDirection(row, col, 1, 0);
        if (verticalWord.length() > 1)
        {
            WordInfo info;
            info.word = verticalWord;
            info.score = 0; // Calculate later
            info.isMainWord = false;
            words.push_back(info);
        }
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

int Board::calculateWordScore(const WordInfo &wordInfo) const
{
    // Simple scoring implementation
    return wordInfo.word.length() * 10;
}
