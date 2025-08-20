// src/Board.cpp - Fix the function signatures to match the header
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
	// Initialize all as NONE first
	for (int r = 0; r < BOARD_SIZE; ++r)
	{
		for (int c = 0; c < BOARD_SIZE; ++c)
		{
			grid[r][c] = Square(NONE);
		}
	}

	// Set center square as START
	grid[7][7] = Square(START);

	// Triple Word squares
	int tw_positions[][2] = {{0, 0}, {0, 7}, {0, 14}, {7, 0}, {7, 14}, {14, 0}, {14, 7}, {14, 14}};
	for (auto pos : tw_positions)
	{
		grid[pos[0]][pos[1]] = Square(TRIPLE_WORD);
	}

	// Double Word squares (diagonals excluding center)
	for (int i = 1; i < 14; i++)
	{
		if (i != 7)
		{
			grid[i][i] = Square(DOUBLE_WORD);
			grid[i][14 - i] = Square(DOUBLE_WORD);
		}
	}

	// Triple Letter squares (standard pattern)
	int tl_positions[][2] = {
		{1, 5}, {1, 9}, {5, 1}, {5, 5}, {5, 9}, {5, 13},
		{9, 1}, {9, 5}, {9, 9}, {9, 13}, {13, 5}, {13, 9}};
	for (auto pos : tl_positions)
	{
		grid[pos[0]][pos[1]] = Square(TRIPLE_LETTER);
	}

	// Double Letter squares (standard pattern)
	int dl_positions[][2] = {
		{0, 3}, {0, 11}, {2, 6}, {2, 8}, {3, 0}, {3, 7}, {3, 14},
		{6, 2}, {6, 6}, {6, 8}, {6, 12}, {7, 3}, {7, 11},
		{8, 2}, {8, 6}, {8, 8}, {8, 12}, {11, 0}, {11, 7}, {11, 14},
		{12, 6}, {12, 8}, {14, 3}, {14, 11}};
	for (auto pos : dl_positions)
	{
		grid[pos[0]][pos[1]] = Square(DOUBLE_LETTER);
	}
}

// ADD noexcept to match header
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
	grid[row][col].premium = NONE;

	return true;
}

// ADD noexcept to match header
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

// ADD noexcept to match header
bool Board::isValidFirstMove(int row, int col) const noexcept
{
	return row == 7 && col == 7; // Center square
}

// ADD noexcept to match header
bool Board::isAdjacentToExistingTile(int row, int col) const noexcept
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

// Fix the function signature to match header
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

Board::Premium Board::getPremium(int row, int col) const
{
	if (!isInBounds(row, col))
		return NONE;
	return grid[row][col].premium;
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
